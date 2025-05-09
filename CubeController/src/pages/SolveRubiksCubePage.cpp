#include "pages/SolveRubiksCubePage.h"
#include "DisplayManager.h"
#include "HttpClientTask.h"
#include "ConvertSolutionTask.h"
#include "MoveExecutionTask.h"
#include <sstream>
#include <iostream>

void SolveRubiksCubePage::show() {
    String url = "http://raspberrypi.local/CubeApi/solve/";
    // test case
    // url = url + "RRBRRBYYYGOOGOOWWWWWWWWWRRBYYYYYYGOOGGRGGRGGRBBBBBBOOO";
    url = url + cubeState.getState();

    int progress = 0;
    HttpClientTask httpTask = HttpClientTask(url);
    httpTask.execute();
    while (!httpTask.hasFinished())
    {
        delay(50);
        DisplayScope scope;
        DisplayManager::printCentered("Getting solution", 20);
        DisplayManager::drawLoading(progress);
        progress = (progress + 10) % 360;
    }
    String httpResponse = httpTask.getResponse();
    if (httpResponse.startsWith("HTTP Error")) {
        DisplayScope scope;
        DisplayManager::printWrapped(httpResponse.c_str());
        hasError = true;
        return;
    }

    ConvertSolutionTask convertTask = ConvertSolutionTask(httpResponse);
    convertTask.execute();
    while (!convertTask.hasFinished())
    {
        delay(50);
        DisplayScope scope;
        DisplayManager::printCentered("Converting solution", 20);
        DisplayManager::drawLoading(progress);
        progress = (progress + 10) % 360;
    }
    std::vector<DMove> moves = convertTask.getResponse();
    MoveExecutionTask moveTask = MoveExecutionTask(moves);
    moveTask.execute();
    while (!moveTask.hasFinished())
    {
        delay(50);
        DisplayScope scope;
        std::string message = std::string("Executing moves ") +
                        std::to_string(moveTask.getMoveIndex()) + "/" +
                        std::to_string(moveTask.getMoveCount());
        DisplayManager::printCentered(message.c_str(), 20);
        DisplayManager::drawLoading(progress);
        progress = (progress + 10) % 360;
    }
    if (moveTask.getMoveIndex() == moveTask.getMoveCount()) {
        DisplayScope scope;
        DisplayManager::printCentered("Cube solved", 20);
        finished = true;
    } else {
        DisplayScope scope;
        DisplayManager::printCentered("Error solving cube", 20);
        hasError = true;
    }
}

void SolveRubiksCubePage::increment(int delta) {
}

std::optional<PagesEnum> SolveRubiksCubePage::press() {
    if (hasError || finished) {
        return PagesEnum::LIST_MENU;
    }
    return std::nullopt;
}