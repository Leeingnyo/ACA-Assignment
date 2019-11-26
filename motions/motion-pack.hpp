#pragma once

#include "motion-clip.hpp"

class MotionPack {
public:
    MotionClip const * motion_clip;
    std::string name;
    int start;
    int length;
    int blend;
    MotionPack const * default_next;

    MotionPack(std::string name, int start, int end, int blend, MotionClip* motion_clip) :
            name(name), start(start), blend(blend), motion_clip(motion_clip) {
        length = end - start;
        default_next = this;
    }
};
