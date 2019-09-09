#include "../camera/camera.h"

class Screen {
private:
    Camera camera;
    int width;
    int height;

    bool is_mouse_left_clicked = false;
    bool is_previous_position_available = false;

    double prev_x;
    double prev_y;
    double prev_z;
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

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
