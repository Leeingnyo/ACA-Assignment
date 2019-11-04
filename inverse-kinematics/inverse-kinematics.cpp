
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

#include "../Eigen/Geometry"

#include "inverse-kinematics.h"
#include "../kinematics/euler-joint/euler-joint.hpp"

#define D 0.0000001

// #define DEBUG(STR) (std::cout << "DEBUG: " << STR << std::endl)
// #define DDEBUG
#define LABEL(STR) std::left << std::setw(10) << (STR)
#define FLEXIBILITY_CONSTANT 1.25

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

void ik_moves(const std::vector<std::tuple<std::shared_ptr<Joint>, std::shared_ptr<Link>, Transform>>& transforms) {
    int r = transforms.size();
    auto joints = std::vector<std::shared_ptr<EulerJoint>>();
    std::vector<int> joints_w;
    auto joint_path_list = std::vector< std::vector<std::pair<std::shared_ptr<Joint>, int>> >();
    auto index_of = [](const auto& joints, const std::shared_ptr<EulerJoint>& euler_joint) {
        int i = 0;
        for (const std::shared_ptr<EulerJoint>& joint : joints) {
            if (joint.get() == euler_joint.get()) return i;
            i++;
        }
        return -1;
    };

    for (const auto& transform : transforms) {
        const auto& root = std::get<0>(transform);
        const auto& end_effector = std::get<1>(transform);
        const auto& target = std::get<2>(transform);

        const auto&& joint_pairs = find_path(root, end_effector);
        joint_path_list.push_back(joint_pairs);

        for (int i = 0; i < joint_pairs.size(); i++) {
            const auto& joint_pair = joint_pairs[i];
            const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint_pair.first);
            int index = index_of(joints, euler_joint);
            int w = joint_pairs.size() - i - 1;
            if (index < 0) {
                joints.push_back(euler_joint);
                joints_w.push_back(w);
            } else {
                if (joints_w[index] < w) {
                    joints_w[index] = w;
                }
            }
        }
    }
    int c = joints.size();
    Eigen::MatrixXd activated_matrix;
    activated_matrix.resize(r, c);
    for (int i = 0; i < r; i++) {
        const auto& joint_pairs = joint_path_list[i];
        auto get_joints = [](const auto& joint_pairs){
            std::vector<std::shared_ptr<EulerJoint>> joints;
            for (const auto& joint_pair : joint_pairs) {
                joints.push_back(std::dynamic_pointer_cast<EulerJoint>(joint_pair.first));
            }
            return joints;
        };
        const auto&& joints_only = get_joints(joint_pairs);
        for (int j = 0; j < c; j++) {
            if (index_of(joints_only, joints[j]) < 0) {
                activated_matrix(i, j) = 0;
            } else {
                activated_matrix(i, j) = 1;
            }
        }
    }

    #ifdef DEBUG
    DEBUG("path and joint matrix found");
    #endif // DEBUG

    if (joints.size() == 0) return;
    static bool is_singular = false;
    unsigned long long steps = 0;
    const auto x_axis = Eigen::Vector4d{1, 0, 0, 1};
    const auto y_axis = Eigen::Vector4d{0, 1, 0, 1};
    const auto z_axis = Eigen::Vector4d{0, 0, 1, 1};
    auto select_axis = [&x_axis, &y_axis, &z_axis](int i) {
        int ii = (i % 3);
        if (ii == 0) return x_axis;
        else if (ii == 1) return y_axis;
        else return z_axis;
    };

    // while (true) {
    for (int cal = 0; cal < 100000; cal++) {
        steps++;
        Eigen::MatrixXd Jacobian = Eigen::MatrixXd::Zero(r * 3, c * 3);
        Eigen::VectorXd delta;
        delta.resize(r * 3);
        std::vector<bool> is_close;
        for (int i = 0; i < r; i++) { // 각 path 마다
            const auto& joint_pairs = joint_path_list[i];
            const auto& transform = transforms[i];
            const auto& root = std::get<0>(transform);
            const auto& end_effector = std::get<1>(transform);
            const auto& target = std::get<2>(transform);
            const int N = 3;

            auto rotation_and_translation_mat_of_joints = std::vector<std::pair<Eigen::Matrix4d, Eigen::Matrix4d>>();
            for (int i = 0; i < joint_pairs.size(); i++) {
                const auto& joint_pair = joint_pairs[i];
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

            if (target.destination) {
                const auto difference = (*target.destination) - end_effector_point;
                is_close.push_back(difference.norm() < 0.02);
                for (int s = 0; s < N; s++) {
                    delta(i * 3 + s) = difference(s);
                }
            }
            else {
                auto difference_orientation = Eigen::AngleAxisd(end_effector_orientation.inverse() * (*target.orientation));
                is_close.push_back(std::cos(difference_orientation.angle()) > 0.98);
                Eigen::Vector3d w = difference_orientation.axis() * difference_orientation.angle();
                for (int s = 0; s < N; s++) {
                    delta(i * 3 + s) = w(s);
                }
            }

            for (int j = 0; j < joint_pairs.size(); j++) {
                const auto& point = points[j];
                const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint_pairs[j].first);
                int jacobian_index = index_of(joints, euler_joint);
                for (int xyz = 0; xyz < 3; xyz++) {
                    const auto angular_velocity = vector4to3(rotation_mat_vec[j] * select_axis(xyz));
                    if (target.destination) {
                        const auto cross = angular_velocity.cross(end_effector_point - point);
                        for (int s = 0; s < N; s++) {
                            Jacobian(i * 3 + s, jacobian_index * 3 + xyz) = cross(s) * pow(FLEXIBILITY_CONSTANT, joints_w[j]);
                        }
                    }
                    else {
                        for (int s = 0; s < N; s++) {
                            Jacobian(i * 3 + s, jacobian_index * 3 + xyz) = angular_velocity(s) * pow(FLEXIBILITY_CONSTANT, joints_w[j]);
                        }
                    }
                }
            }
            // 자코비언 구함
        }

        bool is_close_all = true;
        for (int i = 0; i < is_close.size() && is_close_all; i++) {
            if (!is_close[i]) is_close_all = false;
        }
        if (is_close_all) {
            if (steps != 1) std::cout << steps << std::endl;
            return;
        }
        #ifdef DDEBUG
        std::cout << Jacobian << std::endl << std::endl;
        std::cout << delta.transpose() << std::endl;
        #endif
        #ifdef DEBUG
        DEBUG("jacobian calculated");
        #endif // DEBUG

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Jacobian, Eigen::ComputeThinU | Eigen::ComputeThinV);
        Eigen::VectorXd result = svd.solve(delta);
        #ifdef DDEBUG
        std::cout << LABEL("result") << result.transpose() << std::endl << std::endl;
        #endif
        is_singular = true;
        for (int i = 0; i < result.size(); i++) {
            if (result(i) > 1e-4) is_singular = false;
        }
        if (is_singular) {
            // std::cout << "----------------------------------------------------------------" << std::endl;
            for (int i = 0; i < result.size(); i++) {
                result(i) += 5;
            }
        }
        for (int i = 0; i < joints.size(); i++) {
            const auto& joint = joints[i];
            const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint);
            auto find_index = [](const std::shared_ptr<EulerJoint>& joint, EulerJointChannel channel) {
                for (int i = 0; i < joint->channels.size(); i++) {
                    if (joint->channels[i] == channel) return i;
                }
                return -1;
            };
            int x_index = find_index(euler_joint, EulerJointChannel::X_R);
            if (x_index >= 0) euler_joint->channel_values[x_index] += result(i * 3 + 0) / pow(FLEXIBILITY_CONSTANT, joints_w[i]); // x
            int y_index = find_index(euler_joint, EulerJointChannel::Y_R);
            if (y_index >= 0) euler_joint->channel_values[y_index] += result(i * 3 + 1) / pow(FLEXIBILITY_CONSTANT, joints_w[i]); // y
            int z_index = find_index(euler_joint, EulerJointChannel::Z_R);
            if (z_index >= 0) euler_joint->channel_values[z_index] += result(i * 3 + 2) / pow(FLEXIBILITY_CONSTANT, joints_w[i]); // z
        }
        #ifdef DEBUG
        DEBUG("apply delta");
        #endif // DEBUG
    }
    std::cout << "too many calculation! see you next frame" << std::endl;
}

void ik_move(
        const Eigen::Vector3d& destination,
        const Eigen::Quaterniond& toward,
        const std::shared_ptr<Joint>& root,
        const std::shared_ptr<Link>& end_effector) {
    static bool is_singular = false;
    // for (int k = 0; k < 10; k++) {
    unsigned long long steps = 0;
    auto&& joints = find_path(root, end_effector);
    if (joints.size() == 0) return;
    while (true) {
        steps++;
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
        if (difference.norm() < 0.01 && std::cos(difference_orientation.angle()) > 0.999) {
            if (steps != 1L) {
                std::cout << LABEL("steps") << steps << std::endl;
            }
            return;
        }

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
                #ifdef DDEBUG
                std::cout << xyz << " " << angular_velocity.transpose() << std::endl;
                #endif
                for (int i = N; i < N + A; i++) {
                    Jacobian(i, j * 3 + xyz) = angular_velocity(i - N);
                }
            }
        }
        #ifdef DDEBUG
        std::cout << Jacobian << std::endl << std::endl;
        #endif
        #ifdef DEBUG
        DEBUG("jacobian calculated");
        #endif // DEBUG
        Eigen::VectorXd delta;
        delta.resize(N + A);
        Eigen::Vector3d delta3d = difference * STEP;
        #ifdef DDEBUG
        std::cout << LABEL("delta") << difference.transpose() << std::endl;
        #endif
        for (int i = 0; i < N && i < 3; i++) {
            delta(i) = delta3d(i);
        }
        Eigen::Vector3d w = difference_orientation.axis() * difference_orientation.angle() * STEP;
        #ifdef DDEBUG
        std::cout << LABEL("axis") << difference_orientation.axis().transpose() << std::endl;
        std::cout << LABEL("angle") << difference_orientation.angle() << std::endl;
        std::cout << LABEL("w") << w.transpose() << std::endl;
        #endif
        for (int i = N; i < N + A; i++) {
            delta(i) = w(i - N); // w 넣어야 함
        }
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Jacobian, Eigen::ComputeThinU | Eigen::ComputeThinV);
        Eigen::VectorXd result = svd.solve(delta);
        #ifdef DDEBUG
        std::cout << LABEL("result") << result.transpose() << std::endl << std::endl;
        #endif
        is_singular = true;
        for (int i = 0; i < result.size(); i++) {
            if (result(i) > 1e-4) is_singular = false;
        }
        if (is_singular) {
            // std::cout << "----------------------------------------------------------------" << std::endl;
            for (int i = 0; i < result.size(); i++) {
                result(i) += 1;
            }
        }

        for (int i = 0; i < joints.size(); i++) {
            const auto& joint_pair = joints[i];
            const auto& joint = joint_pair.first;
            const auto& euler_joint = std::dynamic_pointer_cast<EulerJoint>(joint);
            auto find_index = [](const std::shared_ptr<EulerJoint>& joint, EulerJointChannel channel) {
                for (int i = 0; i < joint->channels.size(); i++) {
                    if (joint->channels[i] == channel) return i;
                }
                return -1;
            };
            int x_index = find_index(euler_joint, EulerJointChannel::X_R);
            if (x_index >= 0) euler_joint->channel_values[x_index] += result(i * 3 + 0); // x
            int y_index = find_index(euler_joint, EulerJointChannel::Y_R);
            if (y_index >= 0) euler_joint->channel_values[y_index] += result(i * 3 + 1); // y
            int z_index = find_index(euler_joint, EulerJointChannel::Z_R);
            if (z_index >= 0) euler_joint->channel_values[z_index] += result(i * 3 + 2); // z
        }
        #ifdef DEBUG
        DEBUG("apply delta");
        #endif // DEBUG
    }
}
