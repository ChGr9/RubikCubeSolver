#include "CubeState.h"

CubeState::CubeState() {
    for (int i = 0; i < 54; i++) {
        state[i] = i / 9;
    }
}

void CubeState::increment(int position, int delta) {
    if(position < 0 || position >= state.size()) {
        throw std::invalid_argument("Invalid position " + std::to_string(position));
    }
    delta = delta % 6; // Normalize delta to from -5 to 5
    state[position] = (state[position] + delta + 6) % 6;
}

char CubeState::get(int position) {
    if(position < 0 || position >= state.size()) {
        throw std::invalid_argument("Invalid position " + std::to_string(position));
    }
    return colorMap.at(state[position]);
}

String CubeState::getState() {
    String result = "";
    for (int value : state) {
        result += colorMap.at(value);
    }
    return result;
}