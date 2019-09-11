#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "../GLFW/glfw3.h"

#include "common.hpp"
#include "cylinder.hpp"

void OpenGLCylinder::draw() {
    glPushMatrix();

    float length = glm::length(normal_vector);

    GLfloat rotation_matrix[16];
    getRotationPoint(rotation_matrix, normal_vector, start_point);
    glMultMatrixf(rotation_matrix);

    std::vector<glm::vec3> vertice;
    std::vector<glm::vec3> normals;

    const float unit_theta = 360 / OpenGLCylinder::STEPS;
    for (int i = 0; i <= OpenGLCylinder::STEPS; i++) {
        const float theta = unit_theta * i;
        const float radian = theta / 180 * M_PI;

        const float cos = std::cos(radian);
        const float sin = std::sin(radian);
        vertice.push_back(glm::vec3(cos * radius, sin * radius, 0));
        vertice.push_back(glm::vec3(cos * radius, sin * radius, length));
        normals.push_back(glm::vec3(cos, sin, 0));
        normals.push_back(glm::vec3(cos, sin, 0));
    }

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < OpenGLCylinder::STEPS; i++) {
        const int current = i * 2;
        const int next = (i + 1) * 2;
        const auto& left_top = vertice[current + 1];
        const auto& right_top = vertice[next + 1];
        const auto& left_bottom = vertice[current];
        const auto& right_bottom = vertice[next];

        // 바닥
        glNormal3f(0, 0, -1);
        glVertex3f(0, 0, 0);
        glVertex3f(left_bottom.x, left_bottom.y, left_bottom.z);
        glVertex3f(right_bottom.x, right_bottom.y, right_bottom.z);

        // 옆면 1
        glNormal3f(normals[current + 1].x, normals[current + 1].y, normals[current + 1].z);
        glVertex3f(left_top.x, left_top.y, left_top.z);
        glNormal3f(normals[next + 1].x, normals[next + 1].y, normals[next + 1].z);
        glVertex3f(right_top.x, right_top.y, right_top.z);
        glNormal3f(normals[current].x, normals[current].y, normals[current].z);
        glVertex3f(left_bottom.x, left_bottom.y, left_bottom.z);

        // 옆면 2
        glNormal3f(normals[next + 1].x, normals[next + 1].y, normals[next + 1].z);
        glVertex3f(right_top.x, right_top.y, right_top.z);
        glNormal3f(normals[next].x, normals[next].y, normals[next].z);
        glVertex3f(right_bottom.x, right_bottom.y, right_bottom.z);
        glNormal3f(normals[current].x, normals[current].y, normals[current].z);
        glVertex3f(left_bottom.x, left_bottom.y, left_bottom.z);

        // 덮개
        glNormal3f(0, 0, 1);
        glVertex3f(0, 0, length);
        glVertex3f(right_top.x, right_top.y, right_top.z);
        glVertex3f(left_top.x, left_top.y, left_top.z);
    }

    glEnd();

    glPopMatrix();
}
