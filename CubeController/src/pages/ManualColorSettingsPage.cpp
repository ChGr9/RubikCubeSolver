#include "pages/ManualColorSettingsPage.h"
#include "DisplayManager.h"

void ManualColorSettingsPage::show() {
    DisplayManager::clear();
    DisplayManager::printCentered("Manual Color Settings");
    DisplayManager::sendBuffer();
}

void ManualColorSettingsPage::increment(int delta) {
}

std::optional<PagesEnum> ManualColorSettingsPage::press() {
    return std::nullopt;
}