#include "pages/FaceDetectionPage.h"
#include "DisplayManager.h"

void FaceDetectionPage::show() {
    DisplayManager::clear();
    DisplayManager::printCentered("Face Detection");
    DisplayManager::sendBuffer();
}

void FaceDetectionPage::increment(int delta) {
}

std::optional<PagesEnum> FaceDetectionPage::press() {
    return std::nullopt;
}