#pragma once

#include "../euler-joint/euler-joint.hpp"

class OpenGLEulerJoint : public EulerJoint {
public:
    OpenGLEulerJoint() {
    }
    OpenGLEulerJoint(const std::vector<EulerJointChannel>& channels) : EulerJoint(channels) {
    }

    virtual void draw();
};
