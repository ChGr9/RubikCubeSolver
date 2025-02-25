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
        {
            DisplayScope scope;
            DisplayManager::printCentered("Getting solution", 20);
            DisplayManager::drawLoading(progress);
        }
        progress = (progress + 10) % 360;
    }
    String httpResponse = httpTask.getResponse();

    ConvertSolutionTask convertTask = ConvertSolutionTask(httpResponse);
    convertTask.execute();
    while (!convertTask.hasFinished())
    {
        delay(50);
        {
            DisplayScope scope;
            DisplayManager::printCentered("Converting solution", 20);
            DisplayManager::drawLoading(progress);
        }
        progress = (progress + 10) % 360;
    }
    std::vector<DMove> moves = convertTask.getResponse();
    
    MoveExecutionTask moveTask = MoveExecutionTask(moves);
    moveTask.execute();
    while (!moveTask.hasFinished())
    {
        delay(50);
        {
            DisplayScope scope;
            std::string message = std::string("Executing moves ") +
                          std::to_string(moveTask.getMoveIndex()) + "/" +
                          std::to_string(moveTask.getMoveCount());
            DisplayManager::printCentered(message.c_str(), 20);
            DisplayManager::drawLoading(progress);
        }
        progress = (progress + 10) % 360;
    }
}

void SolveRubiksCubePage::increment(int delta) {
}

std::optional<PagesEnum> SolveRubiksCubePage::press() {
    return std::nullopt;
}