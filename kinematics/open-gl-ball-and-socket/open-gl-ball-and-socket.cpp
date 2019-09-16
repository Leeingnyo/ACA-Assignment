#include "../../GLFW/glfw3.h"
#include "../../glm/glm.hpp"

#include "../../geomatric-primitives/common.hpp"
#include "../../geomatric-primitives/sphere.hpp"

#include "open-gl-ball-and-socket.hpp"

void OpenGLBallAndSocket::draw() {
    glPushMatrix();
    glTranslatef(related_position.x, related_position.y, related_position.z);

    GLfloat matrix[16];
    getRotation(matrix, front, direction);

    glMultMatrixf(matrix);

    glColor3f(1, 0, 0);
    OpenGLSphere sphere = OpenGLSphere(glm::vec3(0, 0, 0), 0.5f);
    sphere.draw();

    if (link) {
        link->draw();
    }

    glPopMatrix();
}
