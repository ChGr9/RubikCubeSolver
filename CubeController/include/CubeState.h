#pragma once
#include <array>
#include <unordered_map>
#include <Arduino.h>

class CubeState {
    private:
        std::array<int, 54> state;
        const std::array<char, 6> colorList = { 'R', 'O', 'B', 'G', 'W', 'Y' };
        const std::unordered_map<char, int> charToIndex;
    public:
        const std::array<String, 6> sides = {
            "Front",
            "Back",
            "Right",
            "Left",
            "Up",
            "Down"
        };
        CubeState();
        ~CubeState() = default;
        void increment(int position, int delta);
        void set(int position, char value);
        char get(int position);
        String getState();
};