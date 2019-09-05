#include <cstdio>
#include <vector>
#include <cmath>
#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

class Camera {
private:
    static constexpr float MAX_FOV = 135.f;
    static constexpr float MIN_FOV = 0.0f;

    glm::vec3 eye = {0, 0, 10};
    glm::vec3 ori = {0, 0, 0}; // at
    glm::vec3 up = {0, 1, 0};
    float fov = 90.f;
    float near = 0.1f;
    float far = 100.f;
public:
    Camera() {
    }

    void setEye(const glm::vec3& eye) {
        this->eye = eye;
    }
    void setUp(const glm::vec3& up) {
        this->up = up;
    }

    const glm::vec3& getEye() const {
        return eye;
    }
    const glm::vec3& getOrigin() const {
        return ori;
    }
    const glm::vec3& getUp() const {
        return up;
    }
    float getFov() const {
        return fov;
    }
    auto getNear() const {
        return near;
    }
    auto getFar() const {
        return far;
    }
};

class Screen {
private:
    Camera camera;
    int width;
    int height;

    bool is_mouse_left_clicked = true;
public:
    static Screen* current_screen;

    Screen() : camera() {
    }

    void setCurrentScreen() {
        current_screen = this;
    }

    const Camera& getCamera() const {
        return camera;
    }

    float getAspect() const {
        return width / (float) height;
    }

    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }

    int* getWidthPointer() {
        return &width;
    }
    int* getHeightPointer() {
        return &height;
    }

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        static double prev_x = 0;
        static double prev_y = 0;
        static double prev_z = 0;

        if (!is_mouse_left_clicked){
            return;
        }

        static glm::quat q(1, 0, 0, 0);

        double x = (xpos - width / 2) / (width / 2);
        double y = -(ypos - height / 2) / (height / 2);
        double r = sqrt(x * x + y * y);
        double z = r >= 1 ? 0 : sqrt(1 - r * r);

        if (!(prev_x == 0 && prev_y == 0)){
            glm::vec3 now(x, y, z);
            glm::vec3 prev(prev_x, prev_y, prev_z);

            now = q * now;
            prev = q * prev;

            glm::vec3 axis = glm::normalize(glm::cross(now, prev));

            float costheta = glm::dot(now, prev) / (glm::length(now) * glm::length(prev));
            float cos = sqrt((costheta + 1) / 2.0f);
            float sin = sqrt((1 - costheta) / 2.0f);

            if (sin < 0.002f){
                sin = 0;
                cos = 1;
            }

            glm::quat q2 = glm::quat(cos, sin * axis.x, sin * axis.y, sin * axis.z);
            glm::vec3 v = camera.getEye() - camera.getOrigin();
            v = q2 * v;
            camera.setEye(v + camera.getOrigin());
            camera.setUp(q2 * camera.getUp());
            q = q2 * q;
        }

        prev_x = x;
        prev_y = y;
        prev_z = z;
    }
};

Screen* Screen::current_screen;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Screen::current_screen->cursor_position_callback(window, xpos, ypos);
}

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
    // glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSwapInterval(1);

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);
    // glDepthFunc(GL_LESS);

    // Light
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
    // glEnable(GL_COLOR_MATERIAL);

    while (!glfwWindowShouldClose(window)) {
        // Render here
        glfwGetFramebufferSize(window, screen.getWidthPointer(), screen.getHeightPointer());
        glViewport(0, 0, screen.getWidth(), screen.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        { // projection
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(screen.getCamera().getFov(), screen.getAspect(), 0.1f, 100.0f);
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

            const glm::vec3& eye = screen.getCamera().getEye();
            const glm::vec3& ori = screen.getCamera().getOrigin();
            const glm::vec3& up = screen.getCamera().getUp();
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
            glVertex3f(0, 1, 0);
            glVertex3f(0, 0, 1);
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

            // Swap front and back buffers
            glfwSwapBuffers(window);
        }

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
