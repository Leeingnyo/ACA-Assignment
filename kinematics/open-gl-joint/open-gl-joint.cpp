#define GLFW_INCLUDE_GLU
#include "../../GLFW/glfw3.h"

#include "open-gl-joint.h"

void OpenGLJoint::draw() {
    glPushMatrix();
    glTranslatef(related_position.x, related_position.y, related_position.z);
    glRotatef(theta, axis.x, axis.y, axis.z);

    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);

    glVertex3f(0, 0, 0);
    glVertex3f(-1, 0, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(0, -1, 0);

    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -1);
    glEnd();

    if (link) {
        link->draw();
    }

    glPopMatrix();
}
