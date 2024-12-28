#pragma once
#include "Page.h"
#include <array>
#include <Arduino.h>

class ListMenuPage : public Page {
    struct MenuItem
    {
        String name;
        PagesEnum page;
    };
    
    private:
    std::array<MenuItem, 3> items;
    int selectedItem;
    public:
    ListMenuPage() {
        items[0] = {"Manual color setting", PagesEnum::MANUAL_COLOR_SETTINGS};
        items[1] = {"Face detection", PagesEnum::FACE_DETECTION};
        items[2] = {"Solve Rubik's Cube", PagesEnum::SOLVE_RUBIKS_CUBE};
        selectedItem = 0;
    }

    void show();
    void increment(int delta);
    std::optional<PagesEnum> press();
};