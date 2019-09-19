#pragma once

#include "../link/link.hpp"

class OpenGLLink : public Link {
public:
    OpenGLLink() {
    }

    OpenGLLink(float l) : Link(l) {
    }

    virtual void draw();
};
