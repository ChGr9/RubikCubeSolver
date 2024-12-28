#include "pages/SolveRubiksCubePage.h"
#include "DisplayManager.h"

void SolveRubiksCubePage::show() {
    DisplayManager::clear();
    DisplayManager::printCentered("Solve Rubik's Cube");
    DisplayManager::sendBuffer();
}

void SolveRubiksCubePage::increment(int delta) {
}

std::optional<PagesEnum> SolveRubiksCubePage::press() {
    return std::nullopt;
}