#pragma once
#include "Page.h"
#include "CubeState.h"
#include "DMove.h"
#include "Move.h"

class SolveRubiksCubePage : public Page {
    private:
        CubeState& cubeState;
        bool hasError = false;
    public:
        SolveRubiksCubePage(CubeState& cubeState) : Page(), cubeState(cubeState) {}
        void show();
        void increment(int delta);
        std::optional<PagesEnum> press();
};