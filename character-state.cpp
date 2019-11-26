#include <iostream>

#include "character-state.hpp"

void CharacterState::input(char key) {
    next_motion = input_map[state][key].first;
    if (next_motion) {
        is_busy = true;
        state = input_map[state][key].second;
        std::cout << "Input: next motion is '" << next_motion->name << "'" << std::endl;
    }
}
