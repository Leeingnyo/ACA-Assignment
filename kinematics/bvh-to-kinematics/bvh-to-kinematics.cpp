#include <iostream>

#include "../../Eigen/Dense"

#include "../../bvh-parser/bvh-parser.hpp"
#include "../../motions/motion-clip.hpp"
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

    const int length = bvh->motion->motion_data.size() / number_of_channels;
    for (int i = 0; i < length; i++) {
        Motion motion;
        motion.position = Eigen::Vector3d{
            bvh->motion->motion_data[i * number_of_channels + 0],
            bvh->motion->motion_data[i * number_of_channels + 1],
            bvh->motion->motion_data[i * number_of_channels + 2]
        };
        // X Y Z 라고 가정
        for (int j = 1; j < number_of_channels / 3; j++) {
            motion.orientations.push_back(Eigen::Quaterniond(
                Eigen::AngleAxisd(bvh->motion->motion_data[i * number_of_channels + j * 3 + 0] * M_PI / 180.0, Eigen::Vector3d{0, 0, 1}) *
                Eigen::AngleAxisd(bvh->motion->motion_data[i * number_of_channels + j * 3 + 1] * M_PI / 180.0, Eigen::Vector3d{1, 0, 0}) *
                Eigen::AngleAxisd(bvh->motion->motion_data[i * number_of_channels + j * 3 + 2] * M_PI / 180.0, Eigen::Vector3d{0, 1, 0})
            ));
            // Z X Y 라고 가정
        }
        root_joint->motion_clip.motions.push_back(motion);
    }
    root_joint->motion_clip.frame_time = bvh->motion->frame_time;

    return root_joint;
}
