#include "../../GLFW/glfw3.h"
#include "../../glm/glm.hpp"

#include "../../geomatric-primitives/common.hpp"
#include "../../geomatric-primitives/sphere.hpp"

#include "open-gl-euler-joint.hpp"

#define SIZE 0.1f

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
    OpenGLSphere sphere = OpenGLSphere(glm::vec3(0, 0, 0), SIZE);
    sphere.draw();

    glBegin(GL_LINES);
    glColor3f(1, 0, 1); // local x
    glVertex3f(0, 0, 0);
    glVertex3f(SIZE + 0.2f, 0, 0);
    glColor3f(1, 1, 0); // local y
    glVertex3f(0, 0, 0);
    glVertex3f(0, SIZE + 0.2f, 0);
    glColor3f(0, 1, 1); // local z
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, SIZE + 0.2f);
    glEnd();

    for (const auto& link : links) {
        link->draw();
    }

    glPopMatrix();
}
