#include <iostream>

#define GLFW_INCLUDE_GLU
#include "../GLFW/glfw3.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"

#include "../character-state.hpp"
#include "screen.h"

Screen* Screen::current_screen;
CharacterState* Screen::character_state;
Motion const * Screen::m;

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

void Screen::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        is_shift = true;
    } else {
        is_shift = false;
    }
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.moveUp();
        if (is_shift) for (int i = 0; i < 9; i++) camera.moveUp();
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.moveLeft();
        if (is_shift) for (int i = 0; i < 9; i++) camera.moveLeft();
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.moveDown();
        if (is_shift) for (int i = 0; i < 9; i++) camera.moveDown();
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.moveRight();
        if (is_shift) for (int i = 0; i < 9; i++) camera.moveRight();
    }

    if (!character_state->is_busy) {
        if (key == GLFW_KEY_W && (action == GLFW_PRESS)) {
            character_state->input('W');
        }
        if (key == GLFW_KEY_Q && (action == GLFW_PRESS)) {
            character_state->input('Q');
        }
        if (key == GLFW_KEY_A && (action == GLFW_PRESS)) {
            character_state->input('A');
        }
        if (key == GLFW_KEY_S && (action == GLFW_PRESS)) {
            character_state->input('S');
        }
        if (key == GLFW_KEY_D && (action == GLFW_PRESS)) {
            character_state->input('D');
        }
        if (key == GLFW_KEY_E && (action == GLFW_PRESS)) {
            character_state->input('E');
        }
        if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS)) {
            character_state->input(' ');
        }
        if (key == GLFW_KEY_X && (action == GLFW_PRESS)) {
            character_state->input('X');
        }
    }

    if (key == GLFW_KEY_V && (action == GLFW_PRESS)) {
        is_blend_mode_off = !is_blend_mode_off;
        std::cout << (is_blend_mode_off ? "Scene: blend mode disabled" : "Scene: blend mode enabled") << std::endl;
    }
    if (key == GLFW_KEY_P && (action == GLFW_PRESS)) {
        camera.setEye(
                    glm::vec3(m->position(0), m->position(1), m->position(2)) * 0.1f - (camera.getOrigin() - camera.getEye())
        );
        camera.setOrigin(
                    glm::vec3(m->position(0), m->position(1), m->position(2)) * 0.1f
        );
        std::cout << "Scene: " << "move camera to player" << std::endl;
    }
    if (key == GLFW_KEY_T && (action == GLFW_PRESS)) {
        if (is_fixed) {
        }
        is_fixed = !is_fixed;
        std::cout << "Scene: " << (is_fixed ? "camera fixed" : "camera unfixed") << std::endl;
    }
}

void Screen::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (is_shift){
        glm::vec3 v = glm::normalize(camera.getOrigin() - camera.getEye());
        camera.setEye(
            camera.getEye() + (float)yoffset * v * 0.1f
        );
        camera.setOrigin(
            camera.getOrigin() + (float)yoffset * v * 0.1f
        );
    }
    else {
        glm::vec3 v = glm::normalize(camera.getOrigin() - camera.getEye());
        camera.setEye(
            camera.getEye() + (float)yoffset * v * 0.1f
        );
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Screen::current_screen->cursor_position_callback(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Screen::current_screen->mouse_button_callback(window, button, action, mods);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Screen::current_screen->key_callback(window, key, scancode, action, mods);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Screen::current_screen->scroll_callback(window, xoffset, yoffset);
}
