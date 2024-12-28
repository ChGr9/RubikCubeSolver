#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <ESP32Encoder.h>
#include <Button2.h>
#include "DisplayManager.h"
#include "ServoManager.h"
#include "PageManager.h"
#include "pages/ListMenuPage.h"

#define ROTARY_PIN_SW   18
#define ROTARY_PIN_DT   19
#define ROTARY_PIN_CLK  21

int lastPosition = 0;

Preferences preferences;
PageManager* pageManager;
ESP32Encoder encoder;
Button2 encoderButton(ROTARY_PIN_SW);

void reconnect() {
  {
    DisplayScope scope;
    DisplayManager::printCentered("Reconnecting to WiFi");
  }
  WiFi.disconnect();
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  int progress = 0;
  while (WiFi.status() != WL_CONNECTED && millis() - start < 30000) { // 30 seconds timeout
    delay(50);
    {
      DisplayScope scope;
      DisplayManager::printCentered("Connecting to WiFi", 20);
      DisplayManager::drawLoading(progress);
    }

    progress = (progress + 10) % 360;
  }
  
  {
    DisplayScope scope;
    if(WiFi.status() == WL_CONNECTED) {
      DisplayManager::printCentered("WiFi reconnected");
    } else {
      DisplayManager::print("Failed to reconnect to WiFi");
    }
  }
}

void encoderLoop() {
  int currentPosition = encoder.getCount();
  if (currentPosition != lastPosition) {
    int delta = currentPosition - lastPosition;
    delta = delta > 0 ? 1 : -1;
    pageManager->increment(delta);
    lastPosition = currentPosition;
  }
}

void setup() {
  Serial.begin(9600);
  DisplayManager::init();
  preferences.begin("network", false);
  String ssid = preferences.getString("ssid", "CYTA_GG");
  String password = preferences.getString("password", "kaniPelares96*99");
  WiFi.begin(ssid, password);
  int progress = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    {
      DisplayScope scope;
      DisplayManager::printCentered("Connecting to WiFi", 20);
      DisplayManager::drawLoading(progress);
    }

    progress = (progress + 10) % 360;
  }
  {
    DisplayScope scope;
    DisplayManager::printCentered("WiFi connected");
  }
  
  ServoManager::init();

  // Rotary encoder
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoder.attachSingleEdge(ROTARY_PIN_DT, ROTARY_PIN_CLK);
  encoder.clearCount();

  pinMode(ROTARY_PIN_SW, INPUT_PULLUP);
  encoderButton.setDebounceTime(50);
  encoderButton.setClickHandler([](Button2 &btn) {
    Serial.println("Button pressed");
    pageManager->press();
  });

  pageManager = new PageManager();
  pageManager->changePage(PagesEnum::LIST_MENU);
}

void loop() {
  delay(10);
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  encoderLoop();
  encoderButton.loop();
}