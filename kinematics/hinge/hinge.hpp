#pragma once

#include <cmath>
#include <memory>

#include "../joint/joint.hpp"

class Hinge : public Joint {
private:
public:
    glm::vec3 axis;
    float angle; // 0 means straight (range: -180 ~ 180)
    float min_angle = -180.f;
    float max_angle = 180.f;

    Hinge() : Joint(), axis(0, 0, 1), angle(0) {
    }

    Hinge(const glm::vec3& _axis, float _angle) : axis(_axis), angle(_angle) {
    }

    Hinge(const glm::vec3& _axis, float _angle, const glm::vec3& r_p) :
            Joint(r_p), axis(_axis), angle(_angle) {
    }

    void setAngle(float new_angle) {
        angle = std::max(min_angle, std::min(max_angle, new_angle));
    }
};
