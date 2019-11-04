#define GLFW_INCLUDE_GLU
#include "../../GLFW/glfw3.h"

#include "../../geomatric-primitives/common.hpp"
#include "../../geomatric-primitives/cylinder.hpp"

#include "open-gl-link.hpp"

#define SIZE 0.1f

void OpenGLLink::draw() {
    glPushMatrix();

    glPushMatrix();
    glColor3f(cylinder_color.x, cylinder_color.y, cylinder_color.z);

    GLfloat matrix[16];
    getRotationPoint(matrix, direction, glm::vec3(0, 0, 0));
    glMultMatrixf(matrix);

    if (length > SIZE) {
        OpenGLCylinder cylinder = OpenGLCylinder(glm::vec3(0, 0, SIZE), glm::vec3(0, 0, length - (joints.size() ? SIZE : 0.f)), SIZE);
        cylinder.draw();
    }
    glPopMatrix();

    glTranslatef(direction.x, direction.y, direction.z);
    for (auto joint : joints) {
        joint->draw();
    }

    glPopMatrix();
}
