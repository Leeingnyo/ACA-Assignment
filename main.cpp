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

#include "geomatric-primitives/cuboid.hpp"
#include "geomatric-primitives/cylinder.hpp"
#include "geomatric-primitives/sphere.hpp"

int main () {
    GLFWwindow* window;
    Screen screen = Screen();
    screen.setCurrentScreen();

    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(960, 720, "Air Dancer", NULL, NULL);
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

    while (!glfwWindowShouldClose(window)) {
        // Render here
        glfwGetFramebufferSize(window, Screen::current_screen->getWidthPointer(),
                               Screen::current_screen->getHeightPointer());
        glViewport(0, 0, Screen::current_screen->getWidth(), Screen::current_screen->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        { // projection
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(Screen::current_screen->getCamera().getFov(),
                           Screen::current_screen->getAspect(), 0.1f, 100.0f);
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

            const glm::vec3& eye = Screen::current_screen->getCamera().getEye();
            const glm::vec3& ori = Screen::current_screen->getCamera().getOrigin();
            const glm::vec3& up = Screen::current_screen->getCamera().getUp();
    	    gluLookAt(eye.x, eye.y, eye.z, ori.x, ori.y, ori.z, up.x, up.y, up.z);

            // glTranslatef(0, -0.5, 0);
            // glShadeModel(GL_SMOOTH);

            // x y
            glColor3f(1, 1, 0);
            glBegin(GL_TRIANGLES);
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);
            glVertex3f(0, 1, 0);
            glEnd();

            // y z
            glColor3f(0, 1, 1);
            glBegin(GL_TRIANGLES);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0);
            glVertex3f(0, 0, 1);
            glEnd();

            // z x
            glColor3f(1, 0, 1);
            glBegin(GL_TRIANGLES);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
            glVertex3f(1, 0, 0);
            glEnd();

            // x y z
            glColor3f(1, 1, 1);
            glBegin(GL_TRIANGLES);
            glVertex3f(1, 0, 0);
            glVertex3f(0, 0, 1);
            glVertex3f(0, 1, 0);
            glEnd();

            glColor3f(1, 0, 0);
            glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(10, 0, 0); // x
            glEnd();
            glColor3f(0, 1, 0);
            glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 10, 0); // y
            glEnd();
            glColor3f(0, 0, 1);
            glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 10); // z
            glEnd();

            /*
            glPushMatrix();
            {
                auto z_base = glm::vec3(0, 0, 1);
                auto y_base = glm::vec3(0, 1, 0);
                auto camera_view = Screen::current_screen->getCamera().getEye()
                        - Screen::current_screen->getCamera().getOrigin();
                auto camera_up = Screen::current_screen->getCamera().getUp();
                glm::quat quaternion;
                { // rotate view
                    auto cross_vector = glm::normalize(glm::cross(z_base, camera_view));
                    float cos_theta = glm::dot(z_base, camera_view) / (glm::length(z_base) * glm::length(camera_view));
                    float tan_theta = sqrt(1 - pow(cos_theta, 2)) / cos_theta;
                    float theta = RADIAN_TO_DEGREE(std::atan(tan_theta));

                    if (!IS_NAN3(cross_vector)) {
                        glRotatef(theta, cross_vector.x, cross_vector.y, cross_vector.z);
                    }

                    float cos_theta_half = sqrt((cos_theta + 1) / 2.0f);
                    float sin_theta_half = sqrt((1 - cos_theta) / 2.0f);
                    quaternion = glm::quat(cos_theta_half, sin_theta_half * cross_vector.x,
                            sin_theta_half * cross_vector.y, sin_theta_half * cross_vector.z);
                }

                glPushMatrix();
                { // rotate up
                    auto rotated_y_base = y_base * quaternion;
                    auto cross_vector = glm::normalize(glm::cross(rotated_y_base, camera_up));
                    float cos_theta = glm::dot(rotated_y_base, camera_up) / (glm::length(rotated_y_base) * glm::length(camera_up));
                    float tan_theta = sqrt(1 - pow(cos_theta, 2)) / cos_theta;
                    float theta = RADIAN_TO_DEGREE(std::atan(tan_theta));
                    printf("(%f, %f, %f) (%f, %f, %f) (%f, %f, %f) (%f, %f, %f) %f\n",
                            cross_vector.x, cross_vector.y, cross_vector.z,
                            glm::normalize(camera_view).x, glm::normalize(camera_view).y, glm::normalize(camera_view).z,
                            glm::normalize(rotated_y_base).x, glm::normalize(rotated_y_base).y, glm::normalize(rotated_y_base).z,
                            glm::normalize(camera_up).x, glm::normalize(camera_up).y, glm::normalize(camera_up).z,
                            theta);

                    if (!IS_NAN3(cross_vector)) {
                        // auto rotated_cross_vector = quaternion * cross_vector;
                        glRotatef(theta, cross_vector.x, cross_vector.y, cross_vector.z);
                    }


                    RGBColor(255, 0, 255);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(2.f, 0.f, 0.f);
                    glVertex3f(1.414f, 1.414f, 0.f);
                    glVertex3f(0.f, 2.f, 0.f);
                    glVertex3f(-1.414f, 1.414f, 0.f);
                    glVertex3f(-2.f, 0.f, 0.f);
                    glEnd();
                }
                glPopMatrix();

                static auto origin = std::chrono::system_clock::now();
                // glRotatef(((std::chrono::system_clock::now() - origin).count() / 1000000) % 360, 0, 0, 1);

                RGBColor(128, 128, 128);
                glBegin(GL_LINE_STRIP);
                glVertex3f(2.f, 0.f, 0.f);
                glVertex3f(1.414f, 1.414f, 0.f);
                glVertex3f(0.f, 2.f, 0.f);
                glVertex3f(-1.414f, 1.414f, 0.f);
                glVertex3f(-2.f, 0.f, 0.f);
                glEnd();
            }
            glPopMatrix();
            */

            // Swap front and back buffers
            glfwSwapBuffers(window);
        }

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
