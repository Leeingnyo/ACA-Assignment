#include "../glm/glm.hpp"

class OpenGLSphere {
public:
    static const int STACKS = 9;
    static const int SECTORS = 36;
    glm::vec3 center;
    float radius;

    OpenGLSphere() : center(0, 0, 0), radius(0) {
    }
    OpenGLSphere(glm::vec3 c, float r) : center(c), radius(r) {
    }

    void draw();
};
