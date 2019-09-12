#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "../GLFW/glfw3.h"

#include "common.hpp"
#include "cuboid.hpp"

void OpenGLCuboid::draw() {
    glPushMatrix();

    GLfloat matrix[16];
    getRotation(matrix, z, y);
    glMultMatrixf(matrix);
    glTranslatef(-width / 2, -height / 2, -length / 2);

    glBegin(GL_TRIANGLES);

    // Back
    glNormal3f(0, 0, -1);
    glVertex3f(0, height, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(width, 0, 0);

    glVertex3f(width, 0, 0);
    glVertex3f(width, height, 0);
    glVertex3f(0, height, 0);

    // Front
    glNormal3f(0, 0, 1);
    glVertex3f(0, height, length);
    glVertex3f(width, height, length);
    glVertex3f(width, 0, length);

    glVertex3f(width, 0, length);
    glVertex3f(0, 0, length);
    glVertex3f(0, height, length);

    // Down
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, length);
    glVertex3f(width, 0, length);

    glVertex3f(width, 0, length);
    glVertex3f(width, 0, 0);
    glVertex3f(0, 0, 0);

    // Up
    glNormal3f(0, 1, 0);
    glVertex3f(0, height, 0);
    glVertex3f(width, height, 0);
    glVertex3f(width, height, length);

    glVertex3f(width, height, length);
    glVertex3f(0, height, length);
    glVertex3f(0, height, 0);

    // Left
    glNormal3f(-1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, height, 0);
    glVertex3f(0, height, length);

    glVertex3f(0, height, length);
    glVertex3f(0, 0, length);
    glVertex3f(0, 0, 0);

    // Right
    glNormal3f(1, 0, 0);
    glVertex3f(width, 0, 0);
    glVertex3f(width, 0, length);
    glVertex3f(width, height, length);

    glVertex3f(width, height, length);
    glVertex3f(width, height, 0);
    glVertex3f(width, 0, 0);

    glEnd();

    glPopMatrix();
}
