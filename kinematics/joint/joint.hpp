#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../drawable.hpp"
#include "../link/link.hpp"

class Link;

class Joint : public Drawable {
private:
public:
    std::shared_ptr<Link> link;
    glm::vec3 related_position;

    Joint() : link(nullptr), related_position(0, 0, 0) {
    }
};
