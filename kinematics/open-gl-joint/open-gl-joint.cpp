#define GLFW_INCLUDE_GLU
#include "../../GLFW/glfw3.h"

#include "../../geomatric-primitives/sphere.hpp"

#include "open-gl-joint.h"

void OpenGLJoint::draw() {
    glPushMatrix();
    glTranslatef(related_position.x, related_position.y, related_position.z);
    glRotatef(theta, axis.x, axis.y, axis.z);

    glColor3f(1, 0, 0);
    OpenGLSphere sphere = OpenGLSphere(glm::vec3(0, 0, 0), 0.5f);
    sphere.draw();

    if (link) {
        link->draw();
    }

    glPopMatrix();
}
