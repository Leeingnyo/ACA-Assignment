
#include <iostream>
#include <vector>
#include <cmath>

#include "../Eigen/Geometry"

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

Eigen::Vector3d vector4to3(const Eigen::Vector4d& vec) {
    return Eigen::Vector3d{vec(0), vec(1), vec(2)};
}

Eigen::Matrix3d matrix4to3(const Eigen::Matrix4d& mat) {
    Eigen::Matrix3d mat3;
    mat3 << mat(0, 0), mat(0, 1), mat(0, 2),
            mat(1, 0), mat(1, 1), mat(1, 2),
            mat(2, 0), mat(2, 1), mat(2, 2);
    return mat3;
}

void ik_move(
        const Eigen::Vector3d& destination,
        const Eigen::Quaterniond& toward,
        std::shared_ptr<Joint>& root,
        std::shared_ptr<Link>& end_effector) {
    for (int k = 0; k < 10; k++) {
        auto&& joints = find_path(root, end_effector);
        if (joints.size() == 0) return;
        #ifdef DEBUG
        DEBUG("path found");
        #endif // DEBUG
        auto rotation_and_translation_mat_of_joints = std::vector<std::pair<Eigen::Matrix4d, Eigen::Matrix4d>>();
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
            rotation_and_translation_mat_of_joints.push_back(std::make_pair(r, t));
        }
        #ifdef DEBUG
        DEBUG("matrix calculated");
        #endif // DEBUG
        auto translation_mat_vec = std::vector<Eigen::Matrix4d>();
        auto rotation_mat_vec = std::vector<Eigen::Matrix4d>();
        for (int i = 0; i < rotation_and_translation_mat_of_joints.size(); i++) {
            const auto& r_t_pair = rotation_and_translation_mat_of_joints[i];
            const auto& rotation_mat = r_t_pair.first;
            const auto& translation_mat = r_t_pair.second;
            rotation_mat_vec.push_back((i == 0 ? Eigen::Matrix4d::Identity() : rotation_mat_vec[i - 1]) * rotation_mat);
            translation_mat_vec.push_back((i == 0 ? Eigen::Matrix4d::Identity() : translation_mat_vec[i - 1]) * rotation_mat * translation_mat);
        }
        auto root_point = Eigen::Vector3d{0, 0, 0};
        auto points = std::vector<Eigen::Vector3d>();
        points.push_back(root_point);
        for (const auto& mat : translation_mat_vec) {
            Eigen::Vector4d calculated = mat * Eigen::Vector4d{0, 0, 0, 1};
            Eigen::Vector3d point3d;
            for (int i = 0; i < 3; i++) point3d(i) = calculated(i);
            points.push_back(point3d);
        }
        auto end_effector_point = points.back();
        points.pop_back();
        auto orientations = std::vector<Eigen::Quaterniond>();
        for (const auto& mat : rotation_mat_vec) {
            Eigen::Quaterniond orientation = Eigen::Quaterniond(matrix4to3(mat));
            orientations.push_back(orientation);
        }
        auto end_effector_orientation = orientations.back();
        #ifdef DEBUG
        DEBUG("points calculated");
        #endif // DEBUG

        const auto difference = destination - end_effector_point;
        auto difference_orientation = Eigen::AngleAxisd(end_effector_orientation.inverse() * toward);
        if (difference.norm() < 0.1 && std::cos(difference_orientation.angle()) > 0.99) {
            return;
        }
        std::cout << "axis " << difference_orientation.axis().transpose() << std::endl;
        std::cout << "angle " << difference_orientation.angle() << std::endl;

        Eigen::MatrixXd Jacobian;
        const int N = 3;
        const int A = 3;
        const double STEP = 0.6;
        Jacobian.resize(N + A, points.size() * 3);
        const auto x_axis = Eigen::Vector4d{1, 0, 0, 1};
        const auto y_axis = Eigen::Vector4d{0, 1, 0, 1};
        const auto z_axis = Eigen::Vector4d{0, 0, 1, 1};
        auto select_axis = [&x_axis, &y_axis, &z_axis](int i) {
            int ii = (i % 3);
            if (ii == 0) return x_axis;
            else if (ii == 1) return y_axis;
            else return z_axis;
        };
        for (int j = 0; j < points.size(); j++) { // TODO 죄다 바꿔야 함
            const auto& point = points[j];
            for (int xyz = 0; xyz < 3; xyz++) {
                const auto angular_velocity = vector4to3(rotation_mat_vec[j] * select_axis(xyz));
                const auto cross = angular_velocity.cross(end_effector_point - point);
                for (int i = 0; i < N; i++) {
                    Jacobian(i, j * 3 + xyz) = cross(i);
                }
                for (int i = N; i < N + A; i++) {
                    Jacobian(i, j * 3 + xyz) = angular_velocity(i - N);
                }
            }
        }
        #ifdef DEBUG
        DEBUG("jacobian calculated");
        #endif // DEBUG
        Eigen::VectorXd delta;
        delta.resize(N + A);
        Eigen::Vector3d delta3d = difference.normalized() * STEP;
        for (int i = 0; i < N && i < 3; i++) {
            delta(i) = delta3d(i);
        }
        Eigen::Vector3d w = difference_orientation.axis() * difference_orientation.angle() * STEP;
        for (int i = N; i < N + A; i++) {
            delta(i) = w(i - N); // w 넣어야 함
        }
        Eigen::VectorXd result = Jacobian.colPivHouseholderQr().solve(delta); // MoorePenrosePseudoinverse(Jacobian) * delta;

        for (int i = 0; i < joints.size(); i++) {
            const auto& joint_pair = joints[i];
            const auto& joint = joint_pair.first;
            const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint);
            euler_joint->channel_values[0] += result(i * 3 + 0); // x
            euler_joint->channel_values[1] += result(i * 3 + 1); // y
            euler_joint->channel_values[2] += result(i * 3 + 2); // z
        }
        #ifdef DEBUG
        DEBUG("apply delta");
        #endif // DEBUG
    }
}
