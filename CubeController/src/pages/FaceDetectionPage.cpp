#include "pages/FaceDetectionPage.h"
#include "DisplayManager.h"
#include "ServoManager.h"
#include "HttpClientTask.h"

void FaceDetectionPage::show() {
    DisplayManager::clear();
    DisplayManager::printCentered("Face Detection");
    DisplayManager::sendBuffer();
}

void FaceDetectionPage::increment(int delta) {
}

std::optional<PagesEnum> FaceDetectionPage::press() {
    if(hasError) {
        return PagesEnum::LIST_MENU;
    }
    int progress = 0;
    Task<String> task("Scan", [this]() {
        return process();
    });
    task.execute();
    while (!task.hasFinished()) {
        delay(50);
        DisplayScope scope;
        DisplayManager::printCentered("Scanning", 20);
        DisplayManager::drawLoading(progress);
        progress = (progress + 10) % 360;
    }
    String result = task.getResponse();
    if (result.isEmpty()) {
        return PagesEnum::LIST_MENU;
    }
    else {
        DisplayScope scope;
        DisplayManager::printCentered(result.c_str(), 20);
        DisplayManager::sendBuffer();
        hasError = true;
        return std::nullopt;
    }
}

String FaceDetectionPage::process(){
    for(int i = 0; i < 6; i++) {
        ServoManager::bootHalfKick();
        String result = scan();
        if (result.startsWith("HTTP Error")) {
            return result;
        }
        result.trim();
        if (result.length() != 9) {
            return "Invalid scan result";
        }
        result = rotate(result, rotation[i]);
        for (int i = 0; i < 9; i++) {
            cubeState.set(faces[i] * 9 + i, result.charAt(i));
        }
        int turn = i % 2 == 0 ? 1 : -1;
        ServoManager::holderTurn(turn);
    }
    return "";
}

String FaceDetectionPage::scan() {
    const String url = "http://raspberrypi.local/CubeApi/scan";
    int progress = 0;
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    String response = "";
    if(httpCode > 0) {
        if(httpCode == HTTP_CODE_OK)
            response = http.getString();
        else
            response = "HTTP Error: " + String(httpCode);
    }
    else
        response = "HTTP Error Code: " + http.errorToString(httpCode);
    http.end();
    return response;
}

String FaceDetectionPage::rotate(String str, int delta) {
    if (str.length() != 9) throw std::invalid_argument("Needs 9‐char string");

    static constexpr std::array<std::array<int,9>,4> idx = {{
        // identity
        {{ 0,1,2, 3,4,5, 6,7,8 }},
        //  90° CW
        {{ 6,3,0, 7,4,1, 8,5,2 }},
        // 180°
        {{ 8,7,6, 5,4,3, 2,1,0 }},
        // 270°
        {{ 2,5,8, 1,4,7, 0,3,6 }}
    }};

    int r = ((delta % 4) + 4) % 4;

    String dst;
    dst.reserve(9);
    for (int i = 0; i < 9; ++i) {
        dst[i] = str[idx[r][i]];
    }
    return dst;
}