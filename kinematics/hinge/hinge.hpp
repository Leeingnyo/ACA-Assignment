#pragma once

#include <cmath>
#include <memory>

#include "../joint/joint.h"

class Hinge : Drawable {
private:
public:
    std::shared_ptr<Joint> joint;
    float angle; // 0 means straight (range: -180 ~ 180)
    float min_angle = -180.f;
    float max_angle = 180.f;

    void setAngle(float new_angle) {
        angle = std::max(min_angle, std::min(max_angle, new_angle));
    }
}
