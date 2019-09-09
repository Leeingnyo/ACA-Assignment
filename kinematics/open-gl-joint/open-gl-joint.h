#pragma once

#include "../joint/joint.h"

class OpenGLJoint : public Joint {
public:
    OpenGLJoint() {
    }

    virtual void draw();
};
