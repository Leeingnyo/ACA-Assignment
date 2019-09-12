#include "../glm/glm.hpp"

class OpenGLCylinder {
public:
    static const int STEPS = 36;
    glm::vec3 start_point;
    glm::vec3 end_point;
    glm::vec3 normal_vector;
    float radius;

    OpenGLCylinder() : start_point(), end_point(), radius(0) {
        normal_vector = end_point - start_point;
    }
    OpenGLCylinder(glm::vec3 sp, glm::vec3 ep, float r) : start_point(sp), end_point(ep), radius(r) {
        normal_vector = end_point - start_point;
    }
    OpenGLCylinder(glm::vec3 sp, glm::vec3 nv, float length, float r) : start_point(sp), radius(r) {
        normal_vector = glm::normalize(nv) * length;
        end_point = start_point + normal_vector;
    }

    void draw();
};
