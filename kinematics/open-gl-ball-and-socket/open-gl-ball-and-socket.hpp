#pragma once

#include "../ball-and-socket/ball-and-socket.hpp"

class OpenGLBallAndSocket : public BallAndSocket {
public:
    OpenGLBallAndSocket() {
    }

    virtual void draw();
};
