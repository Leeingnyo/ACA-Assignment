#include "../../GLFW/glfw3.h"
#include "../../glm/glm.hpp"

#include "../../geomatric-primitives/sphere.hpp"

#include "open-gl-hinge.hpp"

void OpenGLHinge::draw() {
    glPushMatrix();
    glTranslatef(related_position.x, related_position.y, related_position.z);
    glRotatef(angle, axis.x, axis.y, axis.z);

    glColor3f(1, 0, 0);
    OpenGLSphere sphere = OpenGLSphere(glm::vec3(0, 0, 0), 0.5f);
    sphere.draw();

    if (link) {
        link->draw();
    }

    glPopMatrix();
}
