#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../joint/joint.hpp"

enum class EulerJointOrder { XYZ, YZX, ZXY, ZYX, YXZ, XZY };

class EulerJoint : public Joint {
public:
    float x, y, z;
    EulerJointOrder order;

    EulerJoint() : x(0), y(0), z(0), order(EulerJointOrder::XYZ) {
    }
    EulerJoint(const glm::vec3& r_p) : Joint(r_p), x(0), y(0), z(0), order(EulerJointOrder::XYZ) {
    }
    EulerJoint(float _x, float _y, float _z) : x(_x), y(_y), z(_z), order(EulerJointOrder::XYZ) {
    }
    EulerJoint(float _x, float _y, float _z, EulerJointOrder _order) : x(_x), y(_y), z(_z), order(_order) {
    }
    EulerJoint(const glm::vec3& r_p, float _x, float _y, float _z)
            : Joint(r_p), x(_x), y(_y), z(_z), order(EulerJointOrder::XYZ) {
    }
    EulerJoint(const glm::vec3& r_p, float _x, float _y, float _z, EulerJointOrder _order)
            : Joint(r_p), x(_x), y(_y), z(_z), order(_order) {
    }
};
