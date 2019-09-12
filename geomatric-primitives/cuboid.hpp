#include "../glm/glm.hpp"

class OpenGLCuboid {
public:
    glm::vec3 center;
    glm::vec3 x;
    glm::vec3 y;
    glm::vec3 z;
    float width;
    float height;
    float length;

    OpenGLCuboid(glm::vec3 c, glm::vec3 _x, float w, glm::vec3 _y, float h, glm::vec3 _z, float l) :
            center(c), x(_x), y(_y), z(_z), width(w), height(h), length(l) {
    }

    void draw();
};
