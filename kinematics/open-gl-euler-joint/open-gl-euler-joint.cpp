#include "../../GLFW/glfw3.h"
#include "../../glm/glm.hpp"

#include "../../geomatric-primitives/common.hpp"
#include "../../geomatric-primitives/sphere.hpp"

#include "open-gl-euler-joint.hpp"

void OpenGLEulerJoint::draw() {
    glPushMatrix();
    int channel_index = 0;
    for (auto position : channels) {
        switch (position) {
            case EulerJointChannel::X_P: glTranslatef(channel_values[channel_index++], 0, 0); break;
            case EulerJointChannel::Y_P: glTranslatef(0, channel_values[channel_index++], 0); break;
            case EulerJointChannel::Z_P: glTranslatef(0, 0, channel_values[channel_index++]); break;
        }
    }

    for (auto position : channels) {
        switch (position) {
            case EulerJointChannel::X_R: glRotatef(channel_values[channel_index++], 1, 0, 0); break;
            case EulerJointChannel::Y_R: glRotatef(channel_values[channel_index++], 0, 1, 0); break;
            case EulerJointChannel::Z_R: glRotatef(channel_values[channel_index++], 0, 0, 1); break;
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
