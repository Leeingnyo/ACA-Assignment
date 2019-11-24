#pragma once

#include "../glm/glm.hpp"

#define TRANSLATION_UNIT (0.01f)

class Camera {
private:
    static constexpr float MAX_FOV = 135.f;
    static constexpr float MIN_FOV = 0.0f;

    glm::vec3 eye = {0, 0, 20};
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
    void setOrigin(const glm::vec3& origin) {
        this->ori = origin;
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

    void moveUp() {
        auto up_vector = glm::normalize(up) * TRANSLATION_UNIT;
        ori += up_vector;
        eye += up_vector;
    }
    void moveDown() {
        auto up_vector = glm::normalize(up) * TRANSLATION_UNIT;
        ori -= up_vector;
        eye -= up_vector;
    }
    void moveLeft() {
        glm::vec3 view_vector = ori - eye;
        glm::vec3 right_vector = glm::normalize(glm::cross(view_vector, up)) * TRANSLATION_UNIT;

        ori -= right_vector;
        eye -= right_vector;
    }
    void moveRight() {
        glm::vec3 view_vector = ori - eye;
        glm::vec3 right_vector = glm::normalize(glm::cross(view_vector, up)) * TRANSLATION_UNIT;

        ori += right_vector;
        eye += right_vector;
    }
};
