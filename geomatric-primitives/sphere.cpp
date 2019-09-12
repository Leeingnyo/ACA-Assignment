#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "../GLFW/glfw3.h"

#include "sphere.hpp"

#define NORMAL_VERTEX(v) glNormal3f((v).x, (v).y, (v).z)
#define NORMAL_VERTEX_NEGATIVE_Y(v) glNormal3f((v).x, -(v).y, (v).z)

void OpenGLSphere::draw() {
    std::vector<std::vector<glm::vec3>> vertices_list;

    const float phi_unit = 90.f / OpenGLSphere::STACKS;
    const float theta_unit = 360.f / OpenGLSphere::SECTORS;
    for (int i = 0; i < OpenGLSphere::STACKS; i++) {
        const float phi = phi_unit * i;
        const float y = radius * std::sin(phi * M_PI / 180.f);
        const float length = radius * std::cos(phi * M_PI / 180.f);

        std::vector<glm::vec3> vertices;
        for (int j = 0; j <= OpenGLSphere::SECTORS; j++) {
            const float theta = theta_unit * j;
            const float x = length * std::cos(theta * M_PI / 180.f);
            const float z = length * std::sin(theta * M_PI / 180.f);
            vertices.push_back(glm::vec3(x, y, z));
        }
        vertices_list.push_back(vertices);
    }

    const auto& vl = vertices_list;

    // draw
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < OpenGLSphere::STACKS - 1; i++) {
        const auto& former = vl[i];
        const auto& latter = vl[i + 1];
        for (int j = 0; j < OpenGLSphere::SECTORS; j++) {
            const auto& left_bottom = former[j];
            const auto& left_top = latter[j];
            const auto& right_bottom = former[j + 1];
            const auto& right_top = latter[j + 1];

            // positive
            NORMAL_VERTEX(left_top);
            glVertex3f(left_top.x, left_top.y, left_top.z);
            NORMAL_VERTEX(left_bottom);
            glVertex3f(left_bottom.x, left_bottom.y, left_bottom.z);
            NORMAL_VERTEX(right_top);
            glVertex3f(right_top.x, right_top.y, right_top.z);

            NORMAL_VERTEX(right_top);
            glVertex3f(right_top.x, right_top.y, right_top.z);
            NORMAL_VERTEX(left_bottom);
            glVertex3f(left_bottom.x, left_bottom.y, left_bottom.z);
            NORMAL_VERTEX(right_bottom);
            glVertex3f(right_bottom.x, right_bottom.y, right_bottom.z);

            // negative
            NORMAL_VERTEX_NEGATIVE_Y(left_top);
            glVertex3f(left_top.x, -left_top.y, left_top.z);
            NORMAL_VERTEX_NEGATIVE_Y(right_top);
            glVertex3f(right_top.x, -right_top.y, right_top.z);
            NORMAL_VERTEX_NEGATIVE_Y(left_bottom);
            glVertex3f(left_bottom.x, -left_bottom.y, left_bottom.z);

            NORMAL_VERTEX_NEGATIVE_Y(right_top);
            glVertex3f(right_top.x, -right_top.y, right_top.z);
            NORMAL_VERTEX_NEGATIVE_Y(right_bottom);
            glVertex3f(right_bottom.x, -right_bottom.y, right_bottom.z);
            NORMAL_VERTEX_NEGATIVE_Y(left_bottom);
            glVertex3f(left_bottom.x, -left_bottom.y, left_bottom.z);
        }
    }
    glEnd();

    // 덮개
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, radius, 0);
    for (const auto& vertex : vertices_list[vertices_list.size() - 1]) {
        NORMAL_VERTEX(vertex);
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();

    // 깔개
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, -radius, 0);
    for (const auto& vertex : vertices_list[vertices_list.size() - 1]) {
        NORMAL_VERTEX_NEGATIVE_Y(vertex);
        glVertex3f(vertex.x, -vertex.y, vertex.z);
    }
    glEnd();
}
