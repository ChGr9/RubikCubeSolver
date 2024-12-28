#pragma once
#include "Page.h"
#include "CubeState.h"

class ManualColorSettingsPage : public Page {
    private:
        CubeState& cubeState;
        int selectedFace;
        int index;
        bool isCubeletSelected;
    public:
        ManualColorSettingsPage(CubeState& cubeState) : Page(), cubeState(cubeState) {
            selectedFace = 0;
            index = 2;
            isCubeletSelected = false;
        }
        void show();
        void increment(int delta);
        std::optional<PagesEnum> press();
};