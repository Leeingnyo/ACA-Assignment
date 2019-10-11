#pragma once

#include <vector>
#include <memory>

#include "../drawable.hpp"
#include "../joint/joint.hpp"

class Joint;

class Link : public Drawable {
private:
public:
    glm::vec3 direction;
    float length;
    std::vector<std::shared_ptr<Joint>> joints;

    Link() : length(0), direction(0, 1, 0) {
    }

    Link(float l) : length(l), direction(0, l, 0) {
    }

    Link(glm::vec3 d) : direction(d) {
        length = glm::length(direction);
    }

    Link(glm::vec3 d, float l) : length(l) {
        direction = glm::normalize(d) * length;
    }
};
