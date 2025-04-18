#include "CubeState.h"

namespace {
    std::unordered_map<char, int> makeCharToIndex(const std::array<char, 6>& list) {
        std::unordered_map<char, int> map;
        for (int i = 0; i < list.size(); ++i) {
            map[list[i]] = i;
        }
        return map;
    }
}

CubeState::CubeState()
  : charToIndex(makeCharToIndex(colorList))
{
    for (int i = 0; i < 54; ++i) {
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

void CubeState::set(int position, char value) {
    if(position < 0 || position >= state.size()) {
        throw std::invalid_argument("Invalid position " + std::to_string(position));
    }
    auto it = charToIndex.find(value);
    if (it == charToIndex.end())
        throw std::invalid_argument(std::string("Invalid color '") + value + "'");
    state[position] = it->second;
}

char CubeState::get(int position) {
    if(position < 0 || position >= state.size()) {
        throw std::invalid_argument("Invalid position " + std::to_string(position));
    }
    return colorList.at(state[position]);
}

String CubeState::getState() {
    String result = "";
    for (int value : state) {
        result += colorList.at(value);
    }
    return result;
}