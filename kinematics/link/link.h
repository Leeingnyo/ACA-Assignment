#pragma once

#include <vector>
#include <memory>

#include "../drawable.hpp"
#include "../joint/joint.h"

class Joint;

class Link : public Drawable {
private:
public:
    float length;
    std::vector<std::shared_ptr<Joint>> joints;

    Link() : length(0) {
    }
};
