
#include <iostream>
#include <vector>
#include <cmath>

#include "inverse-kinematics.h"

#define D 0.0000001

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
        std::cout << "L" << std::endl;
        joints.push_back(std::make_pair(parent, i));
        if (link == end_effector) {
            found_joints.assign(joints.begin(), joints.end());
        }
        else {
            for (const auto& joint : link->joints) {
                std::cout << "J" << joints.size() << std::endl;
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
