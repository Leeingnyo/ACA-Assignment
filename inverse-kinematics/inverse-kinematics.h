#pragma once

#include <utility>
#include <vector>
#include <memory>
#include "../Eigen/Dense"
#include "../kinematics/joint/joint.hpp"
#include "../kinematics/link/link.hpp"

class Transform {
public:
    std::shared_ptr<Eigen::Quaterniond> orientation;
    std::shared_ptr<Eigen::Vector3d> destination;

    Transform(const Eigen::Quaterniond& orientation) {
        this->orientation = std::make_shared<Eigen::Quaterniond>(orientation);
        this->destination = nullptr;
    }
    Transform(const Eigen::AngleAxisd& orientation) {
        this->orientation = std::make_shared<Eigen::Quaterniond>(Eigen::Quaterniond(orientation));
        this->destination = nullptr;
    }
    Transform(const Eigen::Vector3d destination) {
        this->destination = std::make_shared<Eigen::Vector3d>(destination);
        this->orientation = nullptr;
    }
};

void ik_moves(const std::vector<std::tuple<std::shared_ptr<Joint>, std::shared_ptr<Link>, Transform>>& transforms);
void ik_move(const Eigen::Vector3d& destination, const Eigen::Quaterniond& toward,
        const std::shared_ptr<Joint>& root, const std::shared_ptr<Link>& end_effector);

/*

 Root - Joint1 - Joint2 - Joint3 - End-effector

 theta R x
 theta R y
 theta R z
 theta J1 x
 theta J1 y
 theta J1 z
 theta J2 x
 theta J2 y
 theta J2 z
 theta J3 x
 theta J3 y
 theta J3 z

 End-effector

 x
 y
 z
 theta x
 theta y
 theta z

 J = (
        )

 */
