#pragma once

#include "../euler-joint/euler-joint.hpp"

class OpenGLEulerJoint : public EulerJoint {
public:
    OpenGLEulerJoint() {
    }
    OpenGLEulerJoint(const glm::vec3& related_position) : EulerJoint(related_position) {
    }
    OpenGLEulerJoint(float x, float y, float z) : EulerJoint(x, y, z) {
    }
    OpenGLEulerJoint(float x, float y, float z, EulerJointOrder order) : EulerJoint(x, y, z, order) {
    }
    OpenGLEulerJoint(const glm::vec3& related_position, float x, float y, float z)
            : EulerJoint(related_position, x, y, z) {
    }
    OpenGLEulerJoint(const glm::vec3& related_position, float x, float y, float z, EulerJointOrder order)
            : EulerJoint(related_position, x, y, z, order) {
    }

    virtual void draw();
};
