#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "../GLFW/glfw3.h"

#include "cylinder.hpp"

void getRotation(GLfloat* matrix, glm::vec3 u, glm::vec3 v);

void getRotationPoint(GLfloat* matrix, glm::vec3 u, const glm::vec3& p) {
    glm::vec3 any_point_on_same_plane;

    const bool x_zero = std::abs(u.x - 0.f) < 0.0001f;
    const bool y_zero = std::abs(u.y - 0.f) < 0.0001f;
    const bool z_zero = std::abs(u.z - 0.f) < 0.0001f;

    if (x_zero && y_zero && z_zero) {
        for (int i = 0; i < 16; i++) matrix[i] = 0;
        matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
        return;
    } else if (x_zero && y_zero) {
        any_point_on_same_plane.x = p.x + 1.f;
        any_point_on_same_plane.y = p.y + 1.f;
        any_point_on_same_plane.z = p.z;
    } else if (y_zero && z_zero) {
        any_point_on_same_plane.y = p.y + 1.f;
        any_point_on_same_plane.z = p.z + 1.f;
        any_point_on_same_plane.x = p.x;
    } else if (z_zero && x_zero) {
        any_point_on_same_plane.z = p.z + 1.f;
        any_point_on_same_plane.x = p.x + 1.f;
        any_point_on_same_plane.y = p.y;
    } else if (x_zero) {
        // b y + c z = d
        const float d = u.y * p.y + u.z * p.z;
        any_point_on_same_plane.x = p.x + 1.f;
        any_point_on_same_plane.y = 0;
        any_point_on_same_plane.z = d / u.z;
    } else if (y_zero) {
        const float d = u.z * p.z + u.x * p.x;
        any_point_on_same_plane.x = d / u.x;
        any_point_on_same_plane.y = p.y + 1.f;
        any_point_on_same_plane.z = 0;
    } else if (z_zero) {
        const float d = u.x * p.x + u.y * p.y;
        any_point_on_same_plane.x = 0;
        any_point_on_same_plane.y = d / u.y;
        any_point_on_same_plane.z = p.z + 1.f;
    } else {
        // a x + b y + c z = d
        const float d = u.x * p.x + u.y * p.y + u.z * p.z;
        any_point_on_same_plane.x = p.x + 1.f;
        any_point_on_same_plane.y = 0;
        any_point_on_same_plane.z = (d - u.x * (p.x + 1.f)) / u.z;
    }

    getRotation(matrix, u, any_point_on_same_plane - p);
}

void getRotation(GLfloat* matrix, glm::vec3 u, glm::vec3 v) {
    // https://docs.microsoft.com/en-us/windows/win32/opengl/glmultmatrixf
    u = glm::normalize(u);
    v = glm::normalize(v);
    // suppose z -> u, y -> v
    glm::vec3 w = glm::normalize(glm::cross(v, u));

    matrix[0] = w.x;
    matrix[1] = w.y;
    matrix[2] = w.z;
    matrix[3] = 0;
    matrix[4] = v.x;
    matrix[5] = v.y;
    matrix[6] = v.z;
    matrix[7] = 0;
    matrix[8] = u.x;
    matrix[9] = u.y;
    matrix[10] = u.z;
    matrix[11] = 0;
    matrix[12] = matrix[13] = matrix[14] = 0;
    matrix[15] = 1;
}

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
