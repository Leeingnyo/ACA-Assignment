#include "../../GLFW/glfw3.h"
#include "../../glm/glm.hpp"

#include <cstdio>
#include <memory>

#include "../euler-joint/euler-joint.hpp"

#include "root-joint.hpp"

void RootJoint::animate(const int frame_index, std::shared_ptr<Link> link) {
    for (const auto& joint : link->joints) {
        const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint);
        const int length = euler_joint->channels.size();
        std::vector<float> channel_values;

        for (int i = 0; i < length; i++) {
            channel_values.push_back(animation_information[frame_index * number_of_channels + channel_index++]);
        }

        euler_joint->channel_values = channel_values;

        if (joint->link) {
            animate(frame_index, joint->link);
        }
    }
}

void RootJoint::animate(const int frame_index) {
    channel_index = 0;
    int number = 0;
    if (link) {
        animate(frame_index, link);
    }
}

void RootJoint::draw() {
    glPushMatrix();
    int channel_index = 0;
    for (auto position : channels) {
        switch (position) {
            case EulerJointChannel::X_P: glTranslatef(channel_values[channel_index++], 0, 0); break;
            case EulerJointChannel::Y_P: glTranslatef(0, channel_values[channel_index++], 0); break;
            case EulerJointChannel::Z_P: glTranslatef(0, 0, channel_values[channel_index++]); break;
        }
    }

    for (auto position : channels) {
        switch (position) {
            case EulerJointChannel::X_R: glRotatef(channel_values[channel_index++], 1, 0, 0); break;
            case EulerJointChannel::Y_R: glRotatef(channel_values[channel_index++], 0, 1, 0); break;
            case EulerJointChannel::Z_R: glRotatef(channel_values[channel_index++], 0, 0, 1); break;
        }
    }

    if (link) {
        link->draw();
    }

    glPopMatrix();
}
