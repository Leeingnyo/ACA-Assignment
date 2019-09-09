#define GLFW_INCLUDE_GLU
#include "../../GLFW/glfw3.h"

#include "open-gl-link.h"

void OpenGLLink::draw() {
    glPushMatrix();

    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);
    glEnd();

    glTranslatef(0, length, 0);
    for (auto joint : joints) {
        joint->draw();
    }

    glPopMatrix();
}
