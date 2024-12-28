#include "DisplayManager.h"
#include <Wire.h>
#include <cstdarg>

U8G2_SH1106_128X64_NONAME_F_HW_I2C DisplayManager::display(U8G2_R0, U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

void DisplayManager::init() {
    Wire.begin(I2C_SDA, I2C_SCL);
    display.begin();

    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.setCursor(0, 0);
    display.sendBuffer();
}

void DisplayManager::print(char c, int x, int y, Style style) {
    char text[2] = {c, '\0'};
    drawString(text, x, y, style);
}

void DisplayManager::print(const char* text, int x, int y, Style style) {
    drawString(text, x, y, style);
}

void DisplayManager::printWrapped(const char* text, int x, int y, Style style) {
    u8g2_uint_t lineHeight = display.getAscent() - display.getDescent() + 2;
    u8g2_uint_t cursorX = x;
    u8g2_uint_t cursorY = y;

    const char* ptr = text;

    while (*ptr != '\0') {
        while(*ptr == ' ') {
            ptr++;
        }

        const char* wordEnd = ptr;
        while(*wordEnd != '\0' && *wordEnd != ' ') {
            wordEnd++;
        }

        u8g2_uint_t wordLength = wordEnd - ptr;

        char wordBuffer[128];
        if(wordLength >= sizeof(wordBuffer)) {
            wordLength = sizeof(wordBuffer) - 1;
        }
        strncpy(wordBuffer, ptr, wordLength);
        wordBuffer[wordLength] = '\0';

        u8g2_uint_t wordWidth = display.getStrWidth(wordBuffer);

        if(cursorX + wordWidth > display.getDisplayWidth()) {
            cursorX = x;
            cursorY += lineHeight;
        }

        drawString(wordBuffer, cursorX, cursorY, style);

        cursorX += wordWidth + display.getStrWidth(" ");
        ptr = wordEnd;
    }
}

void DisplayManager::printCentered(const char* text, Style style) {
    u8g2_uint_t screenHeight = display.getDisplayHeight();

    int8_t textAscent = display.getAscent();
    int8_t textDescent = display.getDescent();

    int16_t y = (screenHeight + textAscent - textDescent) / 2 - textDescent;

    printCentered(text, y, style);
}

void DisplayManager::printCentered(const char* text, int y, Style style) {
    u8g2_uint_t screenWidth = display.getDisplayWidth();
    u8g2_uint_t textWidth = display.getStrWidth(text);
    int8_t textAscent = display.getAscent();
    int8_t textDescent = display.getDescent();

    u8g2_uint_t x = (screenWidth - textWidth) / 2;
    x = x < 0 ? 0 : x;

    drawString(text, x, y, style);
}

void DisplayManager::drawLoading(int progress) {
    u8g2_uint_t centerX = display.getDisplayWidth() / 2;
    u8g2_uint_t centerY = 42;
    u8g2_uint_t radius = 15;
    u8g2_uint_t numDots = 4;
    u8g2_uint_t arcLength = 180;

    for(int i = 0; i < numDots; i++) {
        float angle = (progress + i * (arcLength / numDots)) * PI / 180;

        u8g2_uint_t x = centerX + radius * cos(angle);
        u8g2_uint_t y = centerY + radius * sin(angle);

        display.drawDisc(x, y, i);
    }
}

int DisplayManager::getLineHeight() {
    return display.getAscent() - display.getDescent();
}

void DisplayManager::clear() {
    display.clearBuffer();
}

void DisplayManager::sendBuffer() {
    display.sendBuffer();
}

void DisplayManager::drawString(const char* text, int x, int y, Style style) {
    switch (style) {
    case INVERTED:
    {
        const int boxPadding = 1;
        display.drawBox(x - boxPadding, y - display.getAscent() - boxPadding, display.getStrWidth(text) + boxPadding * 2 , getLineHeight() + boxPadding);
        display.setDrawColor(0);
        display.drawStr(x, y, text);
        display.setDrawColor(1);
        break;
    }
    case UNDERLINED:
        display.drawStr(x, y, text);
        display.drawHLine(x, y + 1, display.getStrWidth(text));
        break;
    case NORMAL:
        display.drawStr(x, y, text);
        break;
    default:
        throw std::invalid_argument("Invalid style");
    }
}