#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../drawable.hpp"

class BallAndSocket : Drawable {
public:
    glm::vec3 direction;
    glm::vec3 front;

    void setDirection(glm::vec3 new_direction, glm::vec3 new_front) {
        direction = new_direction;
        front = new_front;
    }
};
