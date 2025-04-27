#pragma once
#include "Page.h"
#include "CubeState.h"

class FaceDetectionPage : public Page {
    private:
        CubeState& cubeState;
        std::array<int, 6> faces = { 4, 1, 2, 5, 0, 3 };
        std::array<int, 6> rotation = { 0, 3, 2, 0, 3, 0 };
        bool hasError = false;
        bool finished = false;
    public:
        FaceDetectionPage(CubeState& cubeState) : Page(), cubeState(cubeState) {}
        void show();
        void increment(int delta);
        std::optional<PagesEnum> press();
    private:
        String process();
        String scan();
        String rotate(String str, int delta);
};