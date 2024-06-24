/**
 * @file view.hpp
 * @brief 此文件管理屏幕上可能显示的任何元素。
 */
#pragma once
#include <string>
#include "./utils/oled.hpp"

namespace view
{
    // 显存区域
    // 空字符串表示该内存未激活
    std::string statusCharging = "";    // 充电详细状态
    std::string statusBarCharging = ""; // 充电状态
    std::string statusBarLogin = "";    // 登录状态，正在登录时使用
    std::string statusBarWireless = ""; // 无线网络状态，在无线网络连接中时使用
    std::string errorCritical = "";     // 严重错误，会在屏幕上显示一个全屏的错误信息

    void drawStatusBar(std::string status)
    {
        oled::u8g2.drawUTF8(0, 0, lang::toString(status));
        oled::u8g2.drawHLine(0, 13, oled::u8g2.getDisplayWidth());
    }

    void mainloop(void)
    {
        if (statusBarCharging != "")
        {
            drawStatusBar(statusBarCharging);
        }
        else if (statusBarLogin != "")
        {
            drawStatusBar(statusBarLogin);
        }
        else if (statusBarWireless != "")
        {
            drawStatusBar(statusBarWireless);
        }
        if (errorCritical != "")
        {
            oled::drawGlobalFatalMsg(errorCritical);
        }
    }
}