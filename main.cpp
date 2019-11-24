#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#define RGBColor(r, g, b) glColor3f((r) / 255.0, (g) / 255.0, (b) / 255.0)
#define RADIAN_TO_DEGREE(rad) ((rad) * 180.f / M_PI)
#define IS_NAN3(vec3) (isnanf((vec3).x) || isnanf((vec3).y) || isnanf((vec3).z))

#include "camera/camera.h"
#include "screen/screen.h"

#include "geomatric-primitives/common.hpp"
#include "geomatric-primitives/cuboid.hpp"
#include "geomatric-primitives/cylinder.hpp"
#include "geomatric-primitives/sphere.hpp"

#include "kinematics/open-gl-hinge/open-gl-hinge.hpp"
#include "kinematics/open-gl-ball-and-socket/open-gl-ball-and-socket.hpp"
#include "kinematics/open-gl-link/open-gl-link.hpp"
#include "kinematics/open-gl-euler-joint/open-gl-euler-joint.hpp"
#include "kinematics/root-joint/root-joint.hpp"
#include "kinematics/human/human.hpp"

#include "kinematics/bvh-to-kinematics/bvh-to-kinematics.hpp"

#include "inverse-kinematics/inverse-kinematics.h"

#include "motions/motion.hpp"

#ifndef __WIN32
#define __int64 long long
#endif // __WIN32

std::shared_ptr<RootJoint> load_motion(inyong_bvh::BvhParser& parser, const char* filename) {
    std::ifstream bvh_file(filename);
    std::string file_content;
    if (bvh_file.is_open()) {
        std::stringstream string_stream;
        string_stream << bvh_file.rdbuf();
        file_content = string_stream.str();
        bvh_file.close();
        auto&& bvh_tokens = parser.scan_from_string(file_content);
        auto&& bvh = parser.parse_from_tokens(bvh_tokens);
        std::shared_ptr<EulerJoint> bvh_kinematics = bvh_to_kinematics(bvh);
        return std::dynamic_pointer_cast<RootJoint>(bvh_kinematics);
    } else {
        std::cout << "Can't open the file \"" << filename << "\"" << std::endl;
        return nullptr;
    }
}

class MotionPack {
public:
    MotionClip* motion_clip;
    std::string name;
    int start;
    int length;
    int blend;
    MotionPack* default_next;

    MotionPack(std::string name, int start, int end, int blend, MotionClip* motion_clip) :
            name(name), start(start), blend(blend), motion_clip(motion_clip) {
        length = end - start;
    }
};

int main (int argc, char* argv[]) {
    GLFWwindow* window;
    Screen screen = Screen();
    screen.setCurrentScreen();

    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(960, 720, "Controllable Character", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);

    // Light
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
    // glEnable(GL_COLOR_MATERIAL);

    inyong_bvh::BvhParser parser = inyong_bvh::BvhParser();

    std::shared_ptr<RootJoint> root = load_motion(parser, "motion-data/stand.bvh");
    std::shared_ptr<RootJoint> stand = load_motion(parser, "motion-data/stand.bvh");
    std::shared_ptr<RootJoint> turn_left = load_motion(parser, "motion-data/turn_left.bvh");
    std::shared_ptr<RootJoint> walk = load_motion(parser, "motion-data/walk.bvh");

    MotionPack p_stand("stand", 0, 20, 20, &stand->motion_clip);
    MotionPack p_walk("walk", 0, 35, 20, &walk->motion_clip);
    MotionPack p_turn_left("turn left", 53, 117, 20, &turn_left->motion_clip);

    p_stand.default_next = &p_stand;
    p_walk.default_next = &p_turn_left;
    p_turn_left.default_next = &p_walk;

    Motion previous_motion;

    MotionPack const * current_motion;
    MotionPack const * next_motion;
    previous_motion = p_stand.motion_clip->motions.back();
    current_motion = &p_stand;
    next_motion = &p_turn_left;
    // get default next function
    // stand -> stand, walk -> walk straight, run -> run straight

    Eigen::Vector3d global_position = previous_motion.position;
    Eigen::Quaterniond global_orientation = previous_motion.orientations[0];

    auto starttime = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(window)) {
        // Render here
        glfwGetFramebufferSize(window, Screen::current_screen->getWidthPointer(),
                Screen::current_screen->getHeightPointer());
        glViewport(0, 0, Screen::current_screen->getWidth(), Screen::current_screen->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        { // animate
            auto current = std::chrono::system_clock::now();
            unsigned __int64 delta_micro = std::chrono::duration_cast<std::chrono::microseconds>(current - starttime).count();
            unsigned __int64 delta_milli = std::chrono::duration_cast<std::chrono::milliseconds>(current - starttime).count();

            int total_frame = current_motion->length;
            int current_frame = (int)(delta_milli / current_motion->motion_clip->frame_time / 1000);
            int offset = current_motion->start;

            if (current_frame < total_frame) {
            } else {
                Motion m = current_motion->motion_clip->motions[current_frame + offset - 1];
                m.position = global_orientation * (m.position - current_motion->motion_clip->motions[offset].position) + global_position;
                m.orientations[0] = global_orientation * m.orientations[0];
                global_position = m.position;
                global_orientation = m.orientations[0];
                auto angle = global_orientation.toRotationMatrix().eulerAngles(1, 0, 2)(0);

                previous_motion = m;
                current_motion = next_motion;
                next_motion = current_motion->default_next;
                starttime = current;
                current_frame = 0;
            }

            Motion m = current_motion->motion_clip->motions[current_frame + offset];
            Motion current_motion_start = current_motion->motion_clip->motions[offset];
            m.position = global_orientation * (m.position - current_motion_start.position) + global_position;
            m.orientations[0] = global_orientation * m.orientations[0];
            current_motion_start.position = global_orientation * (current_motion_start.position - current_motion_start.position) + global_position;
            current_motion_start.orientations[0] = global_orientation * current_motion_start.orientations[0];
            if (current_frame < current_motion->blend) {
                Displacement d = (previous_motion - current_motion_start) *
                        ((std::cos(M_PI / current_motion->blend * current_frame) + 1) / 2.0);
                m = m + d;
            }

            root->animate_with(m);
        }

        { // projection
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(Screen::current_screen->getCamera().getFov(),
                    Screen::current_screen->getAspect(), 0.1f, 100.0f);

            const glm::vec3& eye = Screen::current_screen->getCamera().getEye();
            const glm::vec3& ori = Screen::current_screen->getCamera().getOrigin();
            const glm::vec3& up = Screen::current_screen->getCamera().getUp();
            gluLookAt(eye.x, eye.y, eye.z, ori.x, ori.y, ori.z, up.x, up.y, up.z);
        }

        { // display
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            /*
            GLfloat ambientLight[] = {.8, .8, .8, 1};
            glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
            GLfloat diffuseLight[] = {.2, .2, .2, 1};
            glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
            GLfloat position[] = {1.0, 1.5, 0.0, 0.0};
            glLightfv(GL_LIGHT0, GL_POSITION, position);
            */

            root->draw();

            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3f(-10, 0, 0);
            glVertex3f(10, 0, 0); // x

            glColor3f(0, 1, 0);
            glVertex3f(0, -10, 0);
            glVertex3f(0, 10, 0); // y

            glColor3f(0, 0, 1);
            glVertex3f(0, 0, -10);
            glVertex3f(0, 0, 10); // z
            glEnd();

            /*
            glPushMatrix();
            glTranslated(destination(0), destination(1), destination(2));
            glRotated(Eigen::AngleAxisd(toward).angle() / M_PI * 180,
                    Eigen::AngleAxisd(toward).axis()(0),
                    Eigen::AngleAxisd(toward).axis()(1),
                    Eigen::AngleAxisd(toward).axis()(2));

            glBegin(GL_LINES);
            glColor3f(1, 0, 1);
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0); // x

            glColor3f(1, 1, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0); // y

            glColor3f(0, 1, 1);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1); // z
            glEnd();

            glPopMatrix();
            */

            glPushMatrix();
            { // display object independently of camera
                const auto& origin = Screen::current_screen->getCamera().getOrigin();
                glTranslatef(origin.x, origin.y, origin.z);
                GLfloat matrix[16];
                getRotation(matrix, Screen::current_screen->getCamera().getEye() -
                        Screen::current_screen->getCamera().getOrigin(),
                        Screen::current_screen->getCamera().getUp());
                glMultMatrixf(matrix);


                static auto start = std::chrono::system_clock::now();
                // glRotatef(((std::chrono::system_clock::now() - start).count() / 1000000) % 360, 0, 0, 1);

                RGBColor(255, 255, 255);
                glBegin(GL_LINES);
                {
                    const int STEPS = 180;
                    const float UNIT_OF_THETA = 2 * M_PI / 180;
                    const float LENGTH = 7.5f;
                    for (int i = 0; i < 180; i++) {
                        const float P0 = LENGTH * std::cos(UNIT_OF_THETA * i);
                        const float P1 = LENGTH * std::sin(UNIT_OF_THETA * i);
                        const float Q0 = LENGTH * std::cos(UNIT_OF_THETA * (i +1));
                        const float Q1 = LENGTH * std::sin(UNIT_OF_THETA * (i +1));
                        glVertex3f(P0, P1, 0);
                        glVertex3f(Q0, Q1, 0);
                    }
                }
                glEnd();
            }
            glPopMatrix();

            // Swap front and back buffers
            glfwSwapBuffers(window);
        }

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
