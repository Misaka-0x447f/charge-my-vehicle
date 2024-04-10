#pragma once
#include <map>
#include <string>

namespace buttonState
{
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

    void IRAM_ATTR buttonPressHandler()
    {
        if (digitalRead(A0) == LOW)
        {
            pressedButton = Button::UP;
        }
        else if (digitalRead(A1) == LOW)
        {
            pressedButton = Button::DOWN;
        }
        else if (digitalRead(A2) == LOW)
        {
            pressedButton = buttonState::Button::CANCEL;
        }
        else if (digitalRead(A3) == LOW)
        {
            pressedButton = buttonState::Button::CONFIRM;
        }
        else
        {
            pressedButton = buttonState::Button::Empty;
        }
    }

    void buttonStateListenerInit()
    {
        pinMode(A0, INPUT_PULLUP);
        pinMode(A1, INPUT_PULLUP);
        pinMode(A2, INPUT_PULLUP);
        pinMode(A3, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(A0), buttonPressHandler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(A1), buttonPressHandler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(A2), buttonPressHandler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(A3), buttonPressHandler, CHANGE);
    }
}
