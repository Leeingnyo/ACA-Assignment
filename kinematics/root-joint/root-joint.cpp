#include <cstdio>
#include <memory>

#include "../euler-joint/euler-joint.hpp"

#include "root-joint.hpp"

void RootJoint::animate(const int frame_index, std::shared_ptr<Link> link) {
    for (const auto& joint : link->joints) {
        const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint);

        std::string order_string;
        switch (euler_joint->order) {
            case EulerJointOrder::XYZ: order_string = "xyz"; break;
            case EulerJointOrder::XZY: order_string = "xzy"; break;
            case EulerJointOrder::YXZ: order_string = "yxz"; break;
            case EulerJointOrder::YZX: order_string = "yzx"; break;
            case EulerJointOrder::ZXY: order_string = "zxy"; break;
            case EulerJointOrder::ZYX: order_string = "zyx"; break;
        }
        for (auto character : order_string) {
            printf("%c %f\n", character, animation_information[frame_index * number_of_channels + channel_index]);
            switch (character) {
                case 'x': euler_joint->x = animation_information[frame_index * number_of_channels + channel_index++]; break;
                case 'y': euler_joint->y = animation_information[frame_index * number_of_channels + channel_index++]; break;
                case 'z': euler_joint->z = animation_information[frame_index * number_of_channels + channel_index++]; break;
            }
        }

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
    if (link) {
        link->draw();
    }
}
