#pragma once

#include "../hinge/hinge.hpp"

class OpenGLHinge : public Hinge {
public:
    OpenGLHinge() {
    }

    OpenGLHinge(const glm::vec3& axis, float angle) : Hinge(axis, angle) {
    }

    OpenGLHinge(const glm::vec3& axis, float angle, const glm::vec3& related_position) :
            Hinge(axis, angle, related_position) {
    }

    virtual void draw();
};
