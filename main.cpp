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
    window = glfwCreateWindow(960, 720, "Stretch", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // glfwSetScrollCallback(window, scroll_callback);
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
    /*
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
    auto root = bvh_to_kinematics(bvh);
    */

    auto starttime = std::chrono::system_clock::now();

    std::vector<EulerJointChannel> channels{
        EulerJointChannel::X_R,
        EulerJointChannel::Y_R,
        EulerJointChannel::Z_R
    };

    auto root = std::make_shared<OpenGLEulerJoint>(channels);
    auto link1 = std::make_shared<OpenGLLink>(glm::vec3(3, 0, 0));
    auto joint1 = std::make_shared<OpenGLEulerJoint>(channels);
    auto link2 = std::make_shared<OpenGLLink>(glm::vec3(3, 3, 0));
    auto joint2 = std::make_shared<OpenGLEulerJoint>(channels);
    auto link3 = std::make_shared<OpenGLLink>(glm::vec3(-3, 3, 0));

    std::vector<float> channel_values{ 0.f, 0.f, 0.f };

    root->links.push_back(link1);
    root->channel_values = channel_values;
    link1->joints.push_back(joint1);
    joint1->links.push_back(link2);
    joint1->channel_values = channel_values;
    link2->joints.push_back(joint2);
    joint2->links.push_back(link3);
    joint2->channel_values = channel_values;

    auto&& joints = find_path(root, link3);
    std::cout << joints.size() << std::endl;

    auto mat_vec = std::vector<Eigen::Matrix4d>();
    for (int i = 0; i < joints.size(); i++) {
        const auto& joint_pair = joints[i];
        const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint_pair.first);
        const auto& link = euler_joint->links[joint_pair.second];
        Eigen::Matrix4d r = Eigen::Matrix4d::Identity();
        for (int i = 0; i < euler_joint->channels.size(); i++) {
            Eigen::Matrix4d channel_mat = Eigen::Matrix4d::Identity();
            const double value = euler_joint->channel_values[i];
            const double radian_value = value * M_PI / 180.;
            const auto& channel = euler_joint->channels[i];
            int pos;
            switch (channel) {
                default: continue;
                case EulerJointChannel::X_R: pos = 1; break;
                case EulerJointChannel::Y_R: pos = 2; break;
                case EulerJointChannel::Z_R: pos = 0; break;
            }
            int ppos = (pos + 1) % 3; // 앞의 것
            channel_mat(pos, pos) = channel_mat(ppos, ppos) = std::cos(radian_value);
            channel_mat(pos, ppos) = -1 * (channel_mat(ppos, pos) = std::sin(radian_value));
            r = r * channel_mat;
        }
        Eigen::Matrix4d t;
        t << 1, 0, 0, link->direction.x,
             0, 1, 0, link->direction.y,
             0, 0, 1, link->direction.z,
             0, 0, 0, 1;
        mat_vec.push_back((i == 0 ? Eigen::Matrix4d::Identity() : mat_vec[i - 1]) * r * t);
    }
    auto root_point = Eigen::Vector3d{0, 0, 0};
    auto points = std::vector<Eigen::Vector3d>();
    points.push_back(root_point);
    for (const auto& mat : mat_vec) {
        Eigen::Vector4d calculated = mat * Eigen::Vector4d{0, 0, 0, 1};
        Eigen::Vector3d point3d;
        for (int i = 0; i < 3; i++) point3d(i) = calculated(i);
        points.push_back(point3d);
    }
    auto end_effector_point = points.back();
    points.pop_back();
    Eigen::MatrixXd Jacobian;
    Jacobian.resize(2, points.size() * 1); // TODO 2 -> ?
    const auto z_axis = Eigen::Vector3d{0, 0, 1};
    for (int j = 0; j < points.size() * 1; j++) { // TODO 죄다 바꿔야 함
        const auto& point = points[j];
        const auto cross = z_axis.cross(end_effector_point - point);
        for (int i = 0; i < 2; i++) {
            Jacobian(i, j) = cross(i);
        }
    }
    Eigen::Vector2d delta = Eigen::Vector2d{0.6, -0.6}; // to - from / N
    Eigen::Vector3d result = MoorePenrosePseudoinverse(Jacobian) * delta;
    std::cout << result;
    root->channel_values[2] += result(0, 0) * 10;
    joint1->channel_values[2] += result(1, 0) * 10;
    joint2->channel_values[2] += result(2, 0) * 10;

    // 계산 할 것

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

            // root->animate((int)(delta_milli / bvh->motion->frame_time / 1000) % bvh->motion->number_of_frames);
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
