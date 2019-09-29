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
#include "kinematics/open-gl-euler-joint/open-gl-euler-joint.hpp"
#include "kinematics/root-joint/root-joint.hpp"
#include "kinematics/human/human.hpp"

#include "kinematics/bvh-to-kinematics/bvh-to-kinematics.hpp"

#ifndef __WIN32
#define __int64 long long
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

    const std::string example = ("HIERARCHY\nROOT pelvis\n{\n	OFFSET 0.000000 0.000000 0.000000\n	CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n	JOINT lfemur\n	{\n		OFFSET 11.446000 0.000000 0.000000\n		CHANNELS 3 Zrotation Xrotation Yrotation\n		JOINT ltibia\n		{\n			OFFSET 0.000000 -42.030400 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT lfoot\n			{\n				OFFSET 0.000000 -39.930200 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT ltoes\n				{\n					OFFSET 0.000000 0.000000 11.775800\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					End Site\n					{\n					    OFFSET 0.000000 0.000000 5.000000\n					}\n				}\n			}\n		}\n	}\n	JOINT rfemur\n	{\n		OFFSET -11.446000 0.000000 0.000000\n		CHANNELS 3 Zrotation Xrotation Yrotation\n		JOINT rtibia\n		{\n			OFFSET 0.000000 -42.030400 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT rfoot\n			{\n				OFFSET 0.000000 -39.930200 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT rtoes\n				{\n					OFFSET 0.000000 0.000000 11.775800\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					End Site\n					{\n					    OFFSET 0.000000 0.000000 5.000000\n					}\n				}\n			}\n		}\n	}\n	JOINT thorax\n	{\n		OFFSET 0.000000 11.255600 -5.000000\n		CHANNELS 3 Zrotation Xrotation Yrotation\n		JOINT head\n		{\n			OFFSET 0.000000 43.799300 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			End Site\n			{\n			    OFFSET 0.000000 5.000000 0.000000\n			}\n		}\n		JOINT lclavicle\n		{\n			OFFSET 0.000000 39.863000 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT lhumerus\n			{\n				OFFSET 17.380600 4.958300 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT lradius\n				{\n					OFFSET 0.000000 -27.561300 0.000000\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					JOINT lhand\n					{\n						OFFSET 0.000000 -26.933000 0.000000\n						CHANNELS 3 Zrotation Xrotation Yrotation\n						End Site\n						{\n						    OFFSET 0.000000 -5.000000 0.000000\n						}\n					}\n				}\n			}\n		}\n		JOINT rclavicle\n		{\n			OFFSET 0.000000 39.863000 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT rhumerus\n			{\n				OFFSET -17.380600 4.958300 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT rradius\n				{\n					OFFSET 0.000000 -27.561300 0.000000\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					JOINT rhand\n					{\n						OFFSET 0.000000 -26.933000 0.000000\n						CHANNELS 3 Zrotation Xrotation Yrotation\n						End Site\n						{\n						    OFFSET 0.000000 -5.000000 0.000000\n						}\n					}\n				}\n			}\n		}\n	}\n}\nMOTION\nFrames: 3\nFrame Time: 0.008342\n59.293900 87.796900 4.760200 -0.467276 -1.378122 -89.563821 -5.641307 2.819435 -8.651036 -0.000000 9.240240 -0.000000 0.000000 3.555270 6.177856 0.000000 -1.301336 -0.000000 -1.803037 2.742629 -3.741876 -0.000000 9.274983 -0.000000 0.000000 3.290364 2.906258 0.000000 -29.528157 0.000000 -4.531867 0.132349 0.213401 15.628959 24.623904 -0.608822 0.394562 0.000000 3.487028 83.769402 0.553653 62.001030 -0.000001 -12.781487 -55.037055 5.615823 0.703335 0.000000 0.095169 -0.000000 -0.923419 -78.312883 9.001808 -58.564139 0.000000 -20.003803 36.764996 -3.936911 -2.063559 -0.000000 \n59.291700 87.793600 4.752400 -0.475906 -1.381233 -89.556452 -5.638376 2.830551 -8.625235 0.000000 9.236131 -0.000000 -0.000000 3.523541 6.156061 0.000000 -1.301682 -0.000000 -1.792531 2.748974 -3.691834 -0.000000 9.276495 0.000000 0.000000 3.262773 2.860635 0.000000 -29.527798 0.000000 -4.547519 0.143945 0.207558 15.646355 24.636287 -0.589535 0.469632 -0.000000 3.531537 83.641881 0.487220 62.085857 -0.000000 -12.806394 -55.155542 5.630933 0.721572 0.000000 0.133438 -0.000000 -0.934718 -78.356085 8.980916 -58.621133 0.000000 -19.969767 36.707936 -3.798525 -2.110204 -0.000000");
    inyong_bvh::BvhParser parser = inyong_bvh::BvhParser();
    auto bvh_tokens = parser.scan_from_string(example);
    auto bvh = parser.parse_from_tokens(bvh_tokens);
    auto root = bvh_to_kinematics(bvh);

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

            root->animate((int)(delta_milli / 1000) % 2);
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
