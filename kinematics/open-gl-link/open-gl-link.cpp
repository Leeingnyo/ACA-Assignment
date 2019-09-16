#define GLFW_INCLUDE_GLU
#include "../../GLFW/glfw3.h"

#include "../../geomatric-primitives/cylinder.hpp"

#include "open-gl-link.h"

void OpenGLLink::draw() {
    glPushMatrix();

    glColor3f(1, 1, 1);
    OpenGLCylinder cylinder = OpenGLCylinder(glm::vec3(0, 0.5f, 0), glm::vec3(0, length - 0.5f, 0), 0.5f);
    cylinder.draw();

    glTranslatef(0, length, 0);
    for (auto joint : joints) {
        joint->draw();
    }

    glPopMatrix();
}
