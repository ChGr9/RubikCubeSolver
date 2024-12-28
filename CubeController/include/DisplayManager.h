#pragma once
#include <U8g2lib.h>
#include "pages/Page.h"

class DisplayManager {
    private:
    static const int I2C_SDA = 32;
    static const int I2C_SCL = 33;

    static U8G2_SH1106_128X64_NONAME_F_HW_I2C display;
    public:
    enum Style {
        NORMAL,
        INVERTED,
        UNDERLINED
    };
    static void init();
    static void print(const char* text, int x = 0, int y = 0, Style style = NORMAL);
    static void printWrapped(const char* text, int x = 0, int y = 0, Style style = NORMAL);
    static void printCentered(const char* text, Style style = NORMAL);
    static void printCentered(const char* text, int y, Style style = NORMAL);
    static void drawLoading(int progress);
    static int getLineHeight();
    static void clear();
    static void sendBuffer();
    private:
    static void drawString(const char* text, int x, int y, Style style);
};

class DisplayScope {
    public:
    DisplayScope() {
        DisplayManager::clear();
    }
    ~DisplayScope() {
        DisplayManager::sendBuffer();
    }
};