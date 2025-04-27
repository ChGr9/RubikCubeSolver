#include "pages/FaceDetectionPage.h"
#include "DisplayManager.h"
#include "ServoManager.h"
#include "HttpClientTask.h"

void FaceDetectionPage::show() {
    DisplayManager::clear();
    DisplayManager::printCentered("Face Detection");
    DisplayManager::sendBuffer();
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
        DisplayScope scope;
        DisplayManager::printCentered("Scan complete", 20);
        finished = true;
    }
    else {
        DisplayScope scope;
        DisplayManager::printWrapped(result.c_str());
        DisplayManager::sendBuffer();
        hasError = true;
    }
}

void FaceDetectionPage::increment(int delta) {
}

std::optional<PagesEnum> FaceDetectionPage::press() {
    if(hasError || finished) {
        return PagesEnum::LIST_MENU;
    }
    return std::nullopt;
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
            return "Invalid scan result: length " + String(result.length());
        }
        result = rotate(result, rotation[i]);
        for (int j = 0; j < 9; j++) {
            cubeState.set(faces[i] * 9 + j, result.charAt(j));
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
    if (str.length() != 9) {
        Serial.println("Warning: Invalid string length in rotate: " + String(str.length()));
        return str;
    }

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
        dst += str[idx[r][i]];
    }
    return dst;
}