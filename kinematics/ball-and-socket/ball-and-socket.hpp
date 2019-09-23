#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../joint/joint.hpp"

class BallAndSocket : public Joint {
public:
    glm::vec3 direction;
    glm::vec3 front;

    BallAndSocket() : Joint(), direction(0, 1, 0), front(0, 0, 1) {
    }

    BallAndSocket(const glm::vec3& d, const glm::vec3& f) : direction(d), front(f) {
    }

    BallAndSocket(const glm::vec3& d, const glm::vec3& f, const glm::vec3& r_p) :
            Joint(r_p), direction(d), front(f) {
    }

    void setDirection(glm::vec3 new_direction, glm::vec3 new_front) {
        direction = new_direction;
        front = new_front;
    }
};
