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

    /*
    if (argc <= 1) {
        std::cout << "Please give bvh file path as the first argument" << std::endl;
        return -1;
    }
    std::ifstream bvh_file(argv[1]);
    */

    Motion motion1;
    motion1.position = Eigen::Vector3d{0, 0, 0};
    motion1.orientations.push_back(Eigen::Quaterniond(Eigen::AngleAxisd(0, Eigen::Vector3d(0, 0, 1))));
    std::cout << "motion1" << std::endl;
    std::cout << motion1.position << std::endl;
    std::cout << (motion1.orientations[0]).toRotationMatrix() << std::endl;

    Motion motion2;
    motion2.position = Eigen::Vector3d{10, 0, 0};
    motion2.orientations.push_back(Eigen::Quaterniond(Eigen::AngleAxisd(M_PI / 4, Eigen::Vector3d(0, 0, 1))));
    std::cout << "motion2" << std::endl;
    std::cout << motion2.position << std::endl;
    std::cout << (motion2.orientations[0]).toRotationMatrix() << std::endl;

    Displacement d = motion1 - motion2;
    std::cout << "d = m1 - m2" << std::endl;
    std::cout << d.vectors[0] << std::endl;
    std::cout << Eigen::AngleAxisd(d.vectors[1].norm(), d.vectors[1].normalized()).toRotationMatrix() << std::endl;

    Motion motion3 = motion2 + d;
    std::cout << "motion3 = m2 + d = m2 + (m1 - m2) = m1" << std::endl;
    std::cout << motion3.position << std::endl;
    std::cout << (motion3.orientations[0]).toRotationMatrix() << std::endl;

    Displacement d2 = motion2 - motion1;
    Motion motion4 = motion1 + d2;
    std::cout << "motion4 = m1 + d2 = m1 - (m2 - m1) = m2" << std::endl;
    std::cout << motion4.position << std::endl;
    std::cout << (motion4.orientations[0]).toRotationMatrix() << std::endl;

    std::ifstream bvh_file("Trial001.bvh");
    std::string file_content;
    if (bvh_file.is_open()) {
        std::stringstream string_stream;
        string_stream << bvh_file.rdbuf();
        file_content = string_stream.str();
        bvh_file.close();
    } else {
        std::cout << "Can't open the file" << std::endl;
        return -2;
    }
    inyong_bvh::BvhParser parser = inyong_bvh::BvhParser();
    auto bvh_tokens = parser.scan_from_string(file_content);
    auto bvh = parser.parse_from_tokens(bvh_tokens);
    std::shared_ptr<EulerJoint> bvh_kinematics = bvh_to_kinematics(bvh);
    std::shared_ptr<RootJoint> root = std::dynamic_pointer_cast<RootJoint>(bvh_kinematics);

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

            root->animate((int)(delta_milli / bvh->motion->frame_time / 1000) % bvh->motion->number_of_frames);
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
