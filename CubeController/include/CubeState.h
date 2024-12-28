#pragma once
#include <array>
#include <map>
#include <Arduino.h>

class CubeState {
    private:
        std::array<int, 54> state;
        const std::map<int, char> colorMap = {
            {0, 'R'},
            {1, 'O'},
            {2, 'B'},
            {3, 'G'},
            {4, 'W'},
            {5, 'Y'}
        };
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
        char get(int position);
        String getState();
};