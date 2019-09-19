#pragma once

#include "../ball-and-socket/ball-and-socket.hpp"

class OpenGLBallAndSocket : public BallAndSocket {
public:
    OpenGLBallAndSocket() {
    }

    OpenGLBallAndSocket(const glm::vec3& d, const glm::vec3& f) : BallAndSocket(d, f) {
    }

    OpenGLBallAndSocket(const glm::vec3& d, const glm::vec3& f, const glm::vec3& r_p) :
            BallAndSocket(d, f, r_p) {
    }

    virtual void draw();
};
