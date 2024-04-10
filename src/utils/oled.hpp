#pragma once

#include <Arduino.h>
#include <U8g2lib.h>
#include <functional>
#include <string>
#include "lang.hpp"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

namespace oled {
    U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

    void init(void) {
        u8g2.begin();
    }

    int getUTF8CharBytes(const char* str) {
        unsigned char firstByte = (unsigned char)str[0];
        if (firstByte < 0x80) {
            return 1;
        } else if ((firstByte & 0xE0) == 0xC0) {
            return 2;
        } else if ((firstByte & 0xF0) == 0xE0) {
            return 3;
        } else if ((firstByte & 0xF8) == 0xF0) {
            return 4;
        } else {
            // Invalid UTF-8 character
            return 0;
        }
    }
    /**
     * @brief Draws a UTF-8 string on the display with word wrapping.
     *
     * This function draws a UTF-8 string on the display, automatically wrapping words to the next line
     * when they exceed the display width. It also handles newline characters and ignores any spaces
     * that occur at the start of a new line, whether it's due to a newline character or automatic wrapping.
     *
     * @param marginLeft The left margin (in pixels) where the drawing should start.
     * @param marginRight The right margin (in pixels) where the drawing should end.
     * @param y The y-coordinate (in pixels) where the first line of text should start.
     * @param str The UTF-8 string to be drawn.
     */
    void drawUTF8WithWrap(int marginLeft, int marginRight, int y, const char* str) {
        int startX = marginLeft;
        int displayWidth = u8g2.getDisplayWidth() - marginRight - u8g2.getMaxCharWidth();
        char buffer[displayWidth + 1];
        int bufferWidth = 0;
        int bufferIndex = 0;

        auto resetBufferAndMoveToNextLine = [&]() {
            u8g2.drawUTF8(marginLeft, y, buffer);
            buffer[0] = '\0';
            bufferWidth = 0;
            bufferIndex = 0;
            y += u8g2.getFontAscent() - u8g2.getFontDescent();
            marginLeft = startX;
        };

        while (*str) {
            int charBytes = getUTF8CharBytes(str);
            char currentChar[charBytes + 1];
            strncpy(currentChar, str, charBytes);
            currentChar[charBytes] = '\0';
            int charWidth = u8g2.getUTF8Width(currentChar);

            if (*str == '\n' || marginLeft + bufferWidth + charWidth > displayWidth) {
                if (bufferWidth > 0) {
                    resetBufferAndMoveToNextLine();
                }
                if (*str == '\n') {
                    str++;
                }
                while (*str == ' ') {
                    str++;
                }
                continue;
            }

            strncpy(&buffer[bufferIndex], currentChar, charBytes);
            buffer[bufferIndex + charBytes] = '\0';
            bufferWidth += charWidth;
            bufferIndex += charBytes;
            str += charBytes;
        }

        if (bufferWidth > 0) {
            u8g2.drawUTF8(marginLeft, y, buffer);
        }
    }


    void drawLoop(std::function<void()> func) {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.setFontRefHeightExtendedText();
        u8g2.setDrawColor(1);
        u8g2.setFontPosTop();
        u8g2.setFontDirection(0);
        u8g2.firstPage();
        do {
            func();
        } while (u8g2.nextPage());
    }
}