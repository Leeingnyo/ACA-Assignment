#pragma once

#include <vector>
#include <memory>
#include "../../glm/glm.hpp"

#include "../drawable.hpp"
#include "../link/link.hpp"

class Link;

class Joint : public Drawable {
private:
public:
    std::vector<std::shared_ptr<Link>> links;
    glm::vec3 related_position;

    Joint() : links(), related_position(0, 0, 0) {
    }

    Joint(const glm::vec3 r_p) : links(), related_position(r_p) {
    }
};
