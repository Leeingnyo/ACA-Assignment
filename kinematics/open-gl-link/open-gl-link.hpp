#pragma once

#include "../link/link.hpp"

class OpenGLLink : public Link {
public:
    OpenGLLink() {
    }

    virtual void draw();
};
