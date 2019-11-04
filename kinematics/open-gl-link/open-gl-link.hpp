#pragma once

#include "../link/link.hpp"

class OpenGLLink : public Link {
public:
    glm::vec3 cylinder_color = glm::vec3(1, 1, 1);

    OpenGLLink() {
    }

    OpenGLLink(float l) : Link(l) {
    }

    OpenGLLink(glm::vec3 d) : Link(d) {
    }

    OpenGLLink(glm::vec3 d, float l) : Link(d, l) {
    }

    virtual void draw();
};
