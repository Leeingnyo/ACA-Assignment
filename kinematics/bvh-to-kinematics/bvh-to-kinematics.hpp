#pragma once

#include "../../bvh-parser/bvh-parser.hpp"
#include "../root-joint/root-joint.hpp"

std::shared_ptr<RootJoint> bvh_to_kinematics(const std::shared_ptr<inyong_bvh::Bvh>& bvh);
