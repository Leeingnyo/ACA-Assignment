#define GLFW_INCLUDE_GLU
#include "../../GLFW/glfw3.h"

#include "../../geomatric-primitives/common.hpp"
#include "../../geomatric-primitives/cylinder.hpp"

#include "open-gl-link.hpp"

void OpenGLLink::draw() {
    glPushMatrix();

    glPushMatrix();
    glColor3f(1, 1, 1);

    GLfloat matrix[16];
    getRotationPoint(matrix, direction, glm::vec3(0, 0, 0));
    glMultMatrixf(matrix);

    if (length > 0.5f) {
        OpenGLCylinder cylinder = OpenGLCylinder(glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, length - (joints.size() ? 0.5f : 0.f)), 0.5f);
        cylinder.draw();
    }
    glPopMatrix();

    glTranslatef(direction.x, direction.y, direction.z);
    for (auto joint : joints) {
        joint->draw();
    }

    glPopMatrix();
}
