#include "../open-gl-euler-joint/open-gl-euler-joint.hpp"
#include "../open-gl-link/open-gl-link.hpp"
#include "bvh-to-kinematics.hpp"

std::shared_ptr<EulerJoint> bvh_to_euler_joint(const std::shared_ptr<RootJoint>& root_joint, const std::shared_ptr<inyong_bvh::BvhJoint>& bvh_joint) {
    std::shared_ptr<EulerJoint> euler_joint = std::make_shared<OpenGLEulerJoint>();
    root_joint->number_of_channels += bvh_joint->number_of_channels;

    std::shared_ptr<Link> link = std::make_shared<OpenGLLink>(glm::vec3(bvh_joint->x_offset, bvh_joint->y_offset, bvh_joint->z_offset));
    euler_joint->link = link;
    for (const auto& child_joint : bvh_joint->child_joints) {
        link->joints.push_back(bvh_to_euler_joint(root_joint, child_joint));
    }

    return euler_joint;
}

std::shared_ptr<RootJoint> bvh_to_kinematics(const std::shared_ptr<inyong_bvh::Bvh>& bvh) {
    std::shared_ptr<RootJoint> root_joint = std::make_shared<RootJoint>();
    root_joint->number_of_channels = bvh->hierarchy->root->number_of_channels;
    std::shared_ptr<Link> link = std::make_shared<OpenGLLink>();

    root_joint->link = link;
    for (const auto& child_joint : bvh->hierarchy->root->child_joints) {
        link->joints.push_back(bvh_to_euler_joint(root_joint, child_joint));
    }

    root_joint->animation_information = bvh->motion->motion_data;

    return root_joint;
}
