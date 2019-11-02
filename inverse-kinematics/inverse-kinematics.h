#pragma once

#include <utility>
#include <memory>
#include "../Eigen/Dense"
#include "../kinematics/joint/joint.hpp"
#include "../kinematics/link/link.hpp"

Eigen::MatrixXd MoorePenrosePseudoinverse(Eigen::MatrixXd mat);
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
