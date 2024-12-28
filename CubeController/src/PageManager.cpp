#include "PageManager.h"
#include "pages/ListMenuPage.h"
#include "pages/ManualColorSettingsPage.h"
#include "pages/FaceDetectionPage.h"
#include "pages/SolveRubiksCubePage.h"
#include <Arduino.h>
#include <optional>

PageManager::PageManager(): currentPage(nullptr){
}

std::unique_ptr<Page> PageManager::generatePage(PagesEnum pageType) {
    switch(pageType) {
        case PagesEnum::LIST_MENU:
            return std::make_unique<ListMenuPage>();
        case PagesEnum::FACE_DETECTION:
            return std::make_unique<FaceDetectionPage>();
        case PagesEnum::SOLVE_RUBIKS_CUBE:
            return std::make_unique<SolveRubiksCubePage>();
        case PagesEnum::MANUAL_COLOR_SETTINGS:
            return std::make_unique<ManualColorSettingsPage>();
        default:
            return nullptr;
    }
}

void PageManager::changePage(PagesEnum pageType) {
    auto newPage = generatePage(pageType);
    if(!newPage) {
        Serial.println("Failed to generate page");
        return;
    }
    currentPage = std::move(newPage);
    currentPage->show();
}

void PageManager::increment(int delta) {
    if(currentPage) {
        currentPage->increment(delta);
    }
}

void PageManager::press() {
    if(currentPage) {
        std::optional<PagesEnum> newPageType = currentPage->press();
        if(newPageType) {
            changePage(newPageType.value());
        }
    }
}