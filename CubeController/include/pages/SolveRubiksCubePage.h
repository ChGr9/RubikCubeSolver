#pragma once
#include "Page.h"

class SolveRubiksCubePage : public Page {
    public:
        SolveRubiksCubePage(){}
        void show();
        void increment(int delta);
        std::optional<PagesEnum> press();
};