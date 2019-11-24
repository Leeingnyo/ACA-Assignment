#include "../camera/camera.h"

#include "../motions/motion.hpp"

class Screen {
private:
    Camera camera;
    int width;
    int height;

    bool is_mouse_left_clicked = false;
    bool is_previous_position_available = false;
    bool is_shift = false;
    bool is_fixed = false;

    double prev_x;
    double prev_y;
    double prev_z;
public:
    static Screen* current_screen;
    static Motion const * m;

    Screen() : camera() {
    }

    void setCurrentScreen() {
        current_screen = this;
    }

    const Camera& getCamera() const {
        return camera;
    }

    glm::vec3 getOrigin() const {
        if (is_fixed) return
                        glm::vec3(m->position(0), m->position(1), m->position(2)) * 0.1f;
        return camera.getOrigin();
    }
    glm::vec3 getEye() const {
        if (is_fixed) return camera.getEye() - camera.getOrigin() +
                        glm::vec3(m->position(0), m->position(1), m->position(2)) * 0.1f;
        return camera.getEye();
    }
    glm::vec3 getUp() const {
        return camera.getUp();
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

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
