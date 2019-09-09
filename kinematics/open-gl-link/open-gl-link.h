#pragma once

#include "../link/link.h"

class OpenGLLink : public Link {
public:
    OpenGLLink() {
    }

    virtual void draw();
};
