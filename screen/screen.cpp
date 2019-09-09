#define GLFW_INCLUDE_GLU
#include "../GLFW/glfw3.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"

#include "screen.h"

Screen* Screen::current_screen;

void Screen::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!is_mouse_left_clicked){
        return;
    }

    static glm::quat q(1, 0, 0, 0);

    double unit = width > height ? height / 2.0 : width / 2.0; // small one
    double x = (xpos - width / 2) / unit;
    double y = -(ypos - height / 2) / unit; // normalize
    double r = sqrt(x * x + y * y);
    double z = r >= 1 ? 0 : sqrt(1 - r * r); // get z (on sphere)

    if (is_previous_position_available) {
        glm::vec3 now(x, y, z);
        glm::vec3 prev(prev_x, prev_y, prev_z);

        now = q * now;
        prev = q * prev;

        glm::vec3 axis = glm::normalize(glm::cross(now, prev));

        float costheta = glm::dot(now, prev) / (glm::length(now) * glm::length(prev));
        float cos = sqrt((costheta + 1) / 2.0f);
        float sin = sqrt((1 - costheta) / 2.0f);

        if (sin < 0.002f || isnanf(sin)){
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
    is_previous_position_available = true;
}

void Screen::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        is_mouse_left_clicked = true;
        is_previous_position_available = false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        is_mouse_left_clicked = false;
        is_previous_position_available = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Screen::current_screen->cursor_position_callback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Screen::current_screen->mouse_button_callback(window, button, action, mods);
}