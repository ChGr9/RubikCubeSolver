#pragma once
#include "Task.h"
#include <Arduino.h>
#include <HTTPClient.h>

class HttpClientTask : public Task<String> {
    public:
        HttpClientTask(String url): Task("Http Client Task"), url(url) {}
    private:
        const String url;
        String func() {
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
                response = "HTTP GET failed: " + http.errorToString(httpCode);
            http.end();
            return response;
        }
};