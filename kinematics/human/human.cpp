#include <memory>

#include "human.hpp"

#include "../open-gl-ball-and-socket/open-gl-ball-and-socket.hpp"
#include "../open-gl-hinge/open-gl-hinge.hpp"
#include "../open-gl-link/open-gl-link.hpp"

OpenGLHuman::OpenGLHuman() {
    printf("human!");

    // make instances
    hips = std::make_shared<OpenGLLink>(0.f);

    right_hip_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0));
    right_thigh_link = std::make_shared<OpenGLLink>(3.f);
    right_knee_joint = std::make_shared<OpenGLHinge>(glm::vec3(-1, 0, 0), 0.f);
    right_calf_link = std::make_shared<OpenGLLink>(2.8f);
    right_ankle_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
    right_foot_link = std::make_shared<OpenGLLink>(1.6f);

    left_hip_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
    left_thigh_link = std::make_shared<OpenGLLink>(3.f);
    left_knee_joint = std::make_shared<OpenGLHinge>(glm::vec3(-1, 0, 0), 0.f);
    left_calf_link = std::make_shared<OpenGLLink>(2.8f);
    left_ankle_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
    left_foot_link = std::make_shared<OpenGLLink>(1.6f);

    sacrum_joint = std::make_shared<OpenGLHinge>(glm::vec3(1, 0, 0), 0.f);
    waist_link = std::make_shared<OpenGLLink>(1.2f);
    backbone_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    backbone_link = std::make_shared<OpenGLLink>(2.4f);

    right_shoulder_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0));
    right_upper_arm_link = std::make_shared<OpenGLLink>(2.4f);
    right_elbow_joint = std::make_shared<OpenGLHinge>(glm::vec3(1, 0, 0), 0.f);
    right_lower_arm_link = std::make_shared<OpenGLLink>(2.f);
    right_wrist_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    right_hand_link = std::make_shared<OpenGLLink>(1.2f);

    left_shoulder_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
    left_upper_arm_link = std::make_shared<OpenGLLink>(2.4f);
    left_elbow_joint = std::make_shared<OpenGLHinge>(glm::vec3(1, 0, 0), 0.f);
    left_lower_arm_link = std::make_shared<OpenGLLink>(2.f);
    left_wrist_joint = std::make_shared<OpenGLBallAndSocket>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    left_hand_link = std::make_shared<OpenGLLink>(1.2f);

    neck_joint = std::make_shared<OpenGLBallAndSocket>();
    neck_link = std::make_shared<OpenGLLink>(1.2f);
    head_joint = std::make_shared<OpenGLBallAndSocket>();
    head_link = std::make_shared<OpenGLLink>(1.8f);

    // make forward kinematics map
    hips->joints.push_back(std::static_pointer_cast<Joint>(right_hip_joint));
    hips->joints.push_back(std::static_pointer_cast<Joint>(left_hip_joint));
    hips->joints.push_back(std::static_pointer_cast<Joint>(sacrum_joint));

    right_hip_joint->links.push_back(right_thigh_link);
    right_thigh_link->joints.push_back(std::static_pointer_cast<Joint>(right_knee_joint));
    right_knee_joint->links.push_back(right_calf_link);
    right_calf_link->joints.push_back(std::static_pointer_cast<Joint>(right_ankle_joint));
    right_ankle_joint->links.push_back(right_foot_link);

    left_hip_joint->links.push_back(left_thigh_link);
    left_thigh_link->joints.push_back(std::static_pointer_cast<Joint>(left_knee_joint));
    left_knee_joint->links.push_back(left_calf_link);
    left_calf_link->joints.push_back(std::static_pointer_cast<Joint>(left_ankle_joint));
    left_ankle_joint->links.push_back(left_foot_link);

    sacrum_joint->links.push_back(waist_link);
    waist_link->joints.push_back(std::static_pointer_cast<Joint>(backbone_joint));
    backbone_joint->links.push_back(backbone_link);

    backbone_link->joints.push_back(std::static_pointer_cast<Joint>(right_shoulder_joint));
    backbone_link->joints.push_back(std::static_pointer_cast<Joint>(left_shoulder_joint));
    backbone_link->joints.push_back(std::static_pointer_cast<Joint>(neck_joint));

    right_shoulder_joint->links.push_back(right_upper_arm_link);
    right_upper_arm_link->joints.push_back(std::static_pointer_cast<Joint>(right_elbow_joint));
    right_elbow_joint->links.push_back(right_lower_arm_link);
    right_lower_arm_link->joints.push_back(std::static_pointer_cast<Joint>(right_wrist_joint));
    right_wrist_joint->links.push_back(right_hand_link);

    left_shoulder_joint->links.push_back(left_upper_arm_link);
    left_upper_arm_link->joints.push_back(std::static_pointer_cast<Joint>(left_elbow_joint));
    left_elbow_joint->links.push_back(left_lower_arm_link);
    left_lower_arm_link->joints.push_back(std::static_pointer_cast<Joint>(left_wrist_joint));
    left_wrist_joint->links.push_back(left_hand_link);

    neck_joint->links.push_back(neck_link);
    neck_link->joints.push_back(std::static_pointer_cast<Joint>(head_joint));
    head_joint->links.push_back(head_link);
}

void OpenGLHuman::draw() {
    hips->draw();
}
