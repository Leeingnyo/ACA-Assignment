#include "../../GLFW/glfw3.h"
#include "../../glm/glm.hpp"

#include "../../geomatric-primitives/common.hpp"
#include "../../geomatric-primitives/sphere.hpp"

#include "open-gl-euler-joint.hpp"

void OpenGLEulerJoint::draw() {
    glPushMatrix();
    glTranslatef(related_position.x, related_position.y, related_position.z);

    std::string order_string;
    switch (order) {
        case EulerJointOrder::XYZ: order_string = "xyz"; break;
        case EulerJointOrder::XZY: order_string = "xzy"; break;
        case EulerJointOrder::YXZ: order_string = "yxz"; break;
        case EulerJointOrder::YZX: order_string = "yzx"; break;
        case EulerJointOrder::ZXY: order_string = "zxy"; break;
        case EulerJointOrder::ZYX: order_string = "zyx"; break;
    }
    for (auto character : order_string) {
        switch (character) {
            case 'x': glRotatef(x, 1, 0, 0); break;
            case 'y': glRotatef(y, 0, 1, 0); break;
            case 'z': glRotatef(z, 0, 0, 1); break;
        }
    }

    glColor3f(1, 0, 0);
    OpenGLSphere sphere = OpenGLSphere(glm::vec3(0, 0, 0), 0.5f);
    sphere.draw();

    if (link) {
        link->draw();
    }

    glPopMatrix();
}
