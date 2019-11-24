#pragma once

#include <map>
#include "motions/motion.hpp"
#include "motions/motion-pack.hpp"

enum class State {
    CS_STAND,
    CS_WALK,
    CS_RUN
};

class CharacterState {
public:
    State state = State::CS_STAND;
    Motion previous_motion;
    bool is_busy;
    std::map<State, std::map<char, std::pair<MotionPack const *, State>>> input_map;
    MotionPack const * current_motion;
    MotionPack const * next_motion;

    void input(char key);
};
