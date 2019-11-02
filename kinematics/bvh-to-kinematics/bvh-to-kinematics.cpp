#include "../../bvh-parser/bvh-parser.hpp"
#include "../open-gl-euler-joint/open-gl-euler-joint.hpp"
#include "../open-gl-link/open-gl-link.hpp"
#include "bvh-to-kinematics.hpp"

#define SCALE_RATIO 0.1f

int number_of_channels = 0;

EulerJointChannel transfrom_channel_type(inyong_bvh::BvhChannels channel) {
    switch (channel) {
        case inyong_bvh::BvhChannels::X_P: return EulerJointChannel::X_P;
        case inyong_bvh::BvhChannels::X_R: return EulerJointChannel::X_R;
        case inyong_bvh::BvhChannels::Y_P: return EulerJointChannel::Y_P;
        case inyong_bvh::BvhChannels::Y_R: return EulerJointChannel::Y_R;
        case inyong_bvh::BvhChannels::Z_P: return EulerJointChannel::Z_P;
        case inyong_bvh::BvhChannels::Z_R: return EulerJointChannel::Z_R;
    }
}

std::shared_ptr<Link> bvh_to_euler_joint(const std::shared_ptr<inyong_bvh::BvhJoint>& bvh_joint) {
    auto link = std::make_shared<OpenGLLink>(glm::vec3(bvh_joint->x_offset, bvh_joint->y_offset, bvh_joint->z_offset) * SCALE_RATIO);

    // bvh joint 가 end site 가 아니면
    // joint 를 하나 만들어서 link 에 연결해야 함
    if (bvh_joint->channels.size() == 0) return link;

    auto euler_joint = std::make_shared<OpenGLEulerJoint>();
    for (int i = 0; i < bvh_joint->channels.size(); i++) {
        euler_joint->channels.push_back(transfrom_channel_type(bvh_joint->channels[i]));
        euler_joint->channel_values.push_back(0.f);
        number_of_channels++;
    }
    link->joints.push_back(euler_joint);

    for (const auto& child_joint : bvh_joint->child_joints) {
        euler_joint->links.push_back(bvh_to_euler_joint(child_joint));
    }

    return link;
}

std::shared_ptr<RootJoint> bvh_to_kinematics(const std::shared_ptr<inyong_bvh::Bvh>& bvh) {
    number_of_channels = 0;

    std::shared_ptr<RootJoint> root_joint = std::make_shared<RootJoint>();
    for (int i = 0; i < bvh->hierarchy->root->channels.size(); i++) {
        root_joint->channels.push_back(transfrom_channel_type(bvh->hierarchy->root->channels[i]));
        root_joint->channel_values.push_back(0.f);
        number_of_channels++;
    }

    // child joint 가 있으면 걔의 offset 이 있을테니까 그걸로 link를 만들어서 나한테 넣어줌
    for (const auto& child_joint : bvh->hierarchy->root->child_joints) {
        root_joint->links.push_back(bvh_to_euler_joint(child_joint));
    }

    root_joint->number_of_channels = number_of_channels;
    root_joint->animation_information = bvh->motion->motion_data;

    return root_joint;
}
