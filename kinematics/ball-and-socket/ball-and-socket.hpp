#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../joint/joint.hpp"

class BallAndSocket : Joint {
public:
    glm::vec3 direction;
    glm::vec3 front;

    BallAndSocket() : Joint(), direction(0, 1, 0), front(0, 0, 1) {
    }

    void setDirection(glm::vec3 new_direction, glm::vec3 new_front) {
        direction = new_direction;
        front = new_front;
    }
};
