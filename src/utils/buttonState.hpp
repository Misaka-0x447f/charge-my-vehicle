#pragma once
#include <map>
#include <string>
#include "./eventHandler.hpp"

namespace buttonState
{
    EventHandler confirmButtonEvent;

    enum Button
    {
        Empty,
        UP,
        DOWN,
        CANCEL,
        CONFIRM
    };

    std::map<Button, std::string> buttonNameMap = {
        {Empty, ""},
        {UP, "向上"},
        {DOWN, "向下"},
        {CANCEL, "取消"},
        {CONFIRM, "好"}
    };

    std::string getButtonName(Button buttonName)
    {
        auto it = buttonNameMap.find(buttonName);
        if (it != buttonNameMap.end())
        {
            return it->second;
        }
        else
        {
            return "Invalid button name";
        }
    }

    volatile Button pressedButton = Button::Empty;

    void fireButtonEventInMainLoop() {
        if (pressedButton == Button::UP) {
            // upButtonEvent.dispatch();
        }
        if (pressedButton == Button::DOWN) {
            // downButtonEvent.dispatch();
        }
        if (pressedButton == Button::CANCEL) {
            // cancelButtonEvent.dispatch();
        }
        if (pressedButton == Button::CONFIRM) {
            confirmButtonEvent.dispatch();
        }
        pressedButton = Button::Empty;
    }

    void IRAM_ATTR buttonPressHandler()
    {
        if (digitalRead(A0) == LOW)
        {
            pressedButton = Button::UP;
        }
        if (digitalRead(A1) == LOW)
        {
            pressedButton = Button::DOWN;
        }
        if (digitalRead(A2) == LOW)
        {
            pressedButton = Button::CANCEL;
        }
        if (digitalRead(A3) == LOW)
        {
            pressedButton = Button::CONFIRM;
        }
        // 在 main 中重置被按下的按钮的状态。
    }

    void buttonStateListenerInit()
    {
        pinMode(A0, INPUT);
        pinMode(A1, INPUT);
        pinMode(A2, INPUT);
        pinMode(A3, INPUT);
        attachInterrupt(digitalPinToInterrupt(A0), buttonPressHandler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(A1), buttonPressHandler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(A2), buttonPressHandler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(A3), buttonPressHandler, CHANGE);
    }
}
