#include "pages/ManualColorSettingsPage.h"
#include "DisplayManager.h"

void ManualColorSettingsPage::show() {
    {
        DisplayScope scope;
        const int headerY = 10;
        DisplayManager::printCentered(cubeState.sides[selectedFace].c_str(), headerY);
        DisplayManager::print("<", 10, headerY, index == 0 ? DisplayManager::INVERTED : DisplayManager::NORMAL);
        DisplayManager::print(">", 120, headerY, index == 1 ? DisplayManager::INVERTED : DisplayManager::NORMAL);
        for(int i=0; i<9; i++) {
            int x = 20 + (i % 3) * 40;
            int y = 25 + (i / 3) * 12;
            DisplayManager::print(cubeState.get(selectedFace * 9 + i), x, y, index == i + 2 ? (isCubeletSelected ? DisplayManager::UNDERLINED : DisplayManager::INVERTED) : DisplayManager::NORMAL);
        }
        DisplayManager::printCentered("Back", 60, index == 11 ? DisplayManager::INVERTED : DisplayManager::NORMAL);
    }
}

void ManualColorSettingsPage::increment(int delta) {
    if(isCubeletSelected) {
        cubeState.increment(index - 2, delta); // subtract 2 to account for the first two options being the face selection
    }
    else {
        index = (index + delta + 12) % 12; // 12 = 2 for forward/backward face + 9 faces + 1 for option to go back
    }
    show();
}

std::optional<PagesEnum> ManualColorSettingsPage::press() {
    if(isCubeletSelected) {
        isCubeletSelected = false;
    }
    else {
        if(index < 2){ // face selection
            int delta = index == 0 ? 5 : 1;
            selectedFace = (selectedFace + delta) % 6;
        }
        else if(index == 11) { // back
            return PagesEnum::LIST_MENU;
        }
        else { // cubelet selection
            isCubeletSelected = true;
        }
    }
    show();
    return std::nullopt;
}