#pragma once
#include <optional>
#include "PagesEnum.h"
class PageManager;

class Page {
    public:
    virtual void show() = 0;
    virtual void increment(int delta) = 0;
    virtual std::optional<PagesEnum> press() = 0;
};