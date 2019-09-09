#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../drawable.hpp"
#include "../link/link.h"

class Link;

class Joint : public Drawable {
private:
public:
    std::shared_ptr<Link> link;
    glm::vec3 axis;
    glm::vec3 related_position;
    float theta;

    Joint() : link(nullptr), axis(0, 0, 1), related_position(0, 0, 0), theta(0) {
    }
};
