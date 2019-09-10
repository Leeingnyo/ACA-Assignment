#include <vector>

#include "../GLFW/glfw3.h"

#include "cylinder.hpp"

void OpenGLCylinder::draw() {
    glPushMatrix();

    float length = glm::length(normal_vector);

    glPopMatrix();
}
