#pragma once
#include "Page.h"

class FaceDetectionPage : public Page {
    public:
    FaceDetectionPage() {}
    void show();
    void increment(int delta);
    std::optional<PagesEnum> press();
};