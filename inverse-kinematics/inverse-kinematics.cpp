
#include <iostream>
#include <vector>
#include <cmath>

#include "inverse-kinematics.h"

#include "../kinematics/euler-joint/euler-joint.hpp"

#define D 0.0000001

// #define DEBUG(STR) (std::cout << "DEBUG: " << STR << std::endl)

Eigen::MatrixXd MoorePenrosePseudoinverse(Eigen::MatrixXd mat) {
    auto mt = mat.transpose();
    if (mat.cols() < mat.rows()) {
        // overdeterminent
        auto tt = mt * mat;
        auto tti = tt.inverse();
        if (std::isnan(tti(0, 0)) || std::isinf(tti(0, 0))) {
            const auto I = Eigen::MatrixXd::Identity(tt.rows(), tt.cols());
            return (tt - D * I).inverse() * mt; // ?
        }
        return tti * mt;
    } else {
        auto tt = mat * mt;
        auto tti = tt.inverse();
        if (std::isnan(tti(0, 0)) || std::isinf(tti(0, 0))) {
            const auto I = Eigen::MatrixXd::Identity(tt.rows(), tt.cols());
            return mt * (tt - D * I).inverse();
        }
        return mt * tti;
    }
}

void find_path_internal(
    std::vector<std::pair<std::shared_ptr<Joint>, int>>& found_joints,
    std::vector<std::pair<std::shared_ptr<Joint>, int>>& joints,
    const std::shared_ptr<Joint>& parent,
    const std::shared_ptr<Link>& end_effector
        ) {
    for (int i = 0; i < parent->links.size(); i++) {
        const auto& link = parent->links[i];
        joints.push_back(std::make_pair(parent, i));
        if (link == end_effector) {
            found_joints.assign(joints.begin(), joints.end());
        }
        else {
            for (const auto& joint : link->joints) {
                find_path_internal(
                    found_joints,
                    joints,
                    joint,
                    end_effector
                );
            }
        }
        joints.pop_back();
    }
}

std::vector<std::pair<std::shared_ptr<Joint>, int>>
        find_path(const std::shared_ptr<Joint>& root, const std::shared_ptr<Link>& end_effector) {
    auto found_joints = std::vector<std::pair<std::shared_ptr<Joint>, int>>();
    auto joints = std::vector<std::pair<std::shared_ptr<Joint>, int>>();

    find_path_internal(found_joints, joints, root, end_effector);

    return found_joints;
}

/*
move(to root end-effector )
    20번은 할 거임
        찾아서 점을 계산해본다
        is close ? 0.1 수준 안 함
        자코비언을 구하고 연산해서 더한다
*/

void ik_move(const Eigen::Vector3d& to, std::shared_ptr<Joint>& root, std::shared_ptr<Link>& end_effector) {
    //for (int k = 0; k < 20; k++) {
        auto&& joints = find_path(root, end_effector);
        auto mat_vec = std::vector<Eigen::Matrix4d>();
        #ifdef DEBUG
        DEBUG("path found");
        #endif // DEBUG
        for (int i = 0; i < joints.size(); i++) {
            const auto& joint_pair = joints[i];
            const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint_pair.first);
            const auto& link = euler_joint->links[joint_pair.second];
            Eigen::Matrix4d r = Eigen::Matrix4d::Identity();
            for (int i = 0; i < euler_joint->channels.size(); i++) {
                Eigen::Matrix4d channel_mat = Eigen::Matrix4d::Identity();
                const double value = euler_joint->channel_values[i];
                const double radian_value = value * M_PI / 180.;
                const auto& channel = euler_joint->channels[i];
                int pos;
                switch (channel) {
                    default: continue;
                    case EulerJointChannel::X_R: pos = 1; break;
                    case EulerJointChannel::Y_R: pos = 2; break;
                    case EulerJointChannel::Z_R: pos = 0; break;
                }
                int ppos = (pos + 1) % 3; // 앞의 것
                channel_mat(pos, pos) = channel_mat(ppos, ppos) = std::cos(radian_value);
                channel_mat(pos, ppos) = -1 * (channel_mat(ppos, pos) = std::sin(radian_value));
                r = r * channel_mat;
            }
            Eigen::Matrix4d t;
            t << 1, 0, 0, link->direction.x,
                 0, 1, 0, link->direction.y,
                 0, 0, 1, link->direction.z,
                 0, 0, 0, 1;
            mat_vec.push_back((i == 0 ? Eigen::Matrix4d::Identity() : mat_vec[i - 1]) * r * t);
        }
        #ifdef DEBUG
        DEBUG("matrix calculated");
        #endif // DEBUG
        auto root_point = Eigen::Vector3d{0, 0, 0};
        auto points = std::vector<Eigen::Vector3d>();
        points.push_back(root_point);
        for (const auto& mat : mat_vec) {
            Eigen::Vector4d calculated = mat * Eigen::Vector4d{0, 0, 0, 1};
            Eigen::Vector3d point3d;
            for (int i = 0; i < 3; i++) point3d(i) = calculated(i);
            points.push_back(point3d);
        }
        auto end_effector_point = points.back();
        points.pop_back();
        #ifdef DEBUG
        DEBUG("points calculated");
        #endif // DEBUG

        const auto hello = to - end_effector_point;
        if (hello.norm() < 0.1) {
            //      break;
            return;
        }

        Eigen::MatrixXd Jacobian;
        Jacobian.resize(2, points.size() * 1); // TODO 2 -> ?
        const auto z_axis = Eigen::Vector3d{0, 0, 1};
        for (int j = 0; j < points.size() * 1; j++) { // TODO 죄다 바꿔야 함
            const auto& point = points[j];
            const auto cross = z_axis.cross(end_effector_point - point);
            for (int i = 0; i < 2; i++) {
                Jacobian(i, j) = cross(i);
            }
        }
        #ifdef DEBUG
        DEBUG("jacobian calculated");
        #endif // DEBUG
        Eigen::Vector2d delta; // to - from / N
        Eigen::Vector3d delta3d = (to - end_effector_point).normalized() * 0.6;
        delta(0) = delta3d(0);
        delta(1) = delta3d(1);
        Eigen::VectorXd result = MoorePenrosePseudoinverse(Jacobian) * delta;

        for (int i = 0; i < joints.size(); i++) {
            const auto& joint_pair = joints[i];
            const auto& joint = joint_pair.first;
            const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint);
            euler_joint->channel_values[2] += result(i, 0);
        }
        #ifdef DEBUG
        DEBUG("apply delta");
        #endif // DEBUG
//    }
}
