#pragma once

#include "../glm/glm.hpp"

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
