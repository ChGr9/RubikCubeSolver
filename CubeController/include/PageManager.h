#pragma once
#include "pages/Page.h"
#include "pages/PagesEnum.h"
#include "CubeState.h"
#include <memory>

class PageManager {
    private:
        std::unique_ptr<Page> currentPage;
        CubeState& cubeState;
    public:
        PageManager(CubeState& cubeState);
        ~PageManager() = default;
        void changePage(PagesEnum pageType);
        void increment(int delta);
        void press();
    private:
        std::unique_ptr<Page> generatePage(PagesEnum pageType);
};