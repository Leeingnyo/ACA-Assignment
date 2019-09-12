#include "common.hpp"

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
