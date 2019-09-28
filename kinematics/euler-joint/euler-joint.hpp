#pragma once

#include <memory>
#include "../../glm/glm.hpp"

#include "../joint/joint.hpp"

enum class EulerJointChannel { X_P, Y_P, Z_P, X_R, Y_R, Z_R };

class EulerJoint : public Joint {
public:
    std::vector<float> channel_values;
    std::vector<EulerJointChannel> channels;

    EulerJoint() {
    }
    EulerJoint(const std::vector<EulerJointChannel>& _channels) : channels(_channels) {
    }
};
