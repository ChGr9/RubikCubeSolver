#pragma once
#include "pages/Page.h"
#include "pages/PagesEnum.h"
#include <memory>

class PageManager {
    private:
    std::unique_ptr<Page> currentPage;
    public:
    PageManager();
    ~PageManager() = default;
    void changePage(PagesEnum pageType);
    void increment(int delta);
    void press();
    private:
    std::unique_ptr<Page> generatePage(PagesEnum pageType);
};