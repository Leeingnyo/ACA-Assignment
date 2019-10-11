#pragma once

#include "../link/link.hpp"

class OpenGLLink : public Link {
public:
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
