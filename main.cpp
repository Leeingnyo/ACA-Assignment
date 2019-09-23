#include <cstdio>
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
#include "kinematics/human/human.hpp"

#ifndef __WIN32
#define __int64 long long;
#endif // __WIN32

int main () {
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

    OpenGLHuman human = OpenGLHuman();

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

            const float shoulder_angle = 30.f;
            const float elbow_angle = 120.f;
            human.right_shoulder_joint->direction = glm::vec3(-std::cos(shoulder_angle * M_PI / 180), 0, std::sin(shoulder_angle * M_PI / 180));
            human.right_shoulder_joint->front = glm::vec3(0, 0, 1);
            human.right_elbow_joint->angle = elbow_angle;
            human.left_shoulder_joint->direction = glm::vec3(std::cos(shoulder_angle * M_PI / 180), 0, std::sin(shoulder_angle * M_PI / 180));
            human.left_shoulder_joint->front = glm::vec3(0, 0, 1);
            human.left_elbow_joint->angle = elbow_angle;

            const float one_cycle_per_second = delta_milli * 360 / 1000.f;
            const float one_radian_per_second = one_cycle_per_second * M_PI / 180.f;
            const float& x = one_radian_per_second;
            const float sin_x = std::sin(x);
            const float cos_x = std::cos(x);
            const float cycle = std::sin(x / 2);
            const float cycle_half = std::sin(x / 4);
            human.backbone_joint->front = glm::vec3(std::sin(cycle * 120 * M_PI / 180),
                    0, std::cos(cycle * 120 * M_PI / 180));
            human.backbone_joint->direction  = glm::vec3(std::sin(cycle * 10 * M_PI / 180),
                    std::cos(cycle * 10 * M_PI / 180), 0);

            human.right_hip_joint->direction = glm::vec3(
                    -std::sin(-std::max(-cycle * 30, 0.f) * M_PI / 180),
                    -std::cos(std::max(-cycle * 120, 0.f) * M_PI / 180),
                    std::sin(std::max(-cycle * 120, 0.f) * M_PI / 180));
            human.right_hip_joint->front = glm::vec3(
                    std::sin(std::max(-cycle * 30, 0.f) * M_PI / 180),
                    std::sin(std::max(-cycle * 90, 0.f) * M_PI / 180),
                    std::cos(std::max(-cycle * 90, 0.f) * M_PI / 180));
            human.right_knee_joint->angle = std::max(-cycle * 150, 0.f);

            human.left_hip_joint->direction = glm::vec3(
                    std::sin(-std::max(cycle * 30, 0.f) * M_PI / 180),
                    -std::cos(std::max(cycle * 120, 0.f) * M_PI / 180),
                    std::sin(std::max(cycle * 120, 0.f) * M_PI / 180));
            human.left_hip_joint->front = glm::vec3(
                    -std::sin(std::max(cycle * 30, 0.f) * M_PI / 180),
                    std::sin(std::max(cycle * 90, 0.f) * M_PI / 180),
                    std::cos(std::max(cycle * 90, 0.f) * M_PI / 180));
            human.left_knee_joint->angle = std::max(cycle * 150, 0.f);
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

            human.draw();

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
