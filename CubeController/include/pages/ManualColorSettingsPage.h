#pragma once
#include "Page.h"

class ManualColorSettingsPage : public Page {
    public:
    ManualColorSettingsPage() : Page() {}
    void show();
    void increment(int delta);
    std::optional<PagesEnum> press();
};