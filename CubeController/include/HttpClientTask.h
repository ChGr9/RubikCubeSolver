#pragma once
#include "Task.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

class HttpClientTask : public Task<String> {
    public:
        HttpClientTask(String url): Task("Http Client Task", [this]{
            return func();
        }), url(url) {}
    private:
        const String url;
        String func() {
            WiFiClient client;
            HTTPClient http;
            
            // Set increased timeouts
            http.setConnectTimeout(10000);
            http.setTimeout(30000);
            client.setTimeout(30000);
            
            // Begin with explicit client for better timeout control
            http.begin(client, url);
            
            int httpCode = http.GET();
            String response = "";
            
            if(httpCode > 0) {
                if(httpCode == HTTP_CODE_OK)
                    response = http.getString();
                else
                    response = "HTTP Error: " + String(httpCode);
            } else {
                response = "HTTP Error Code: " + http.errorToString(httpCode);
            }
            
            http.end();
            return response;
        }
};