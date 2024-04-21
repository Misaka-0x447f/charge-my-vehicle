#pragma once
#include <map>
#include <string>
#include "./eventHandler.hpp"
#include <functional>
#include <vector>

namespace buttonState
{
    EventHandler upButtonEvent;
    EventHandler downButtonEvent;
    EventHandler cancelButtonEvent;
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
        {CONFIRM, "好"}};

    struct ButtonData
    {
        Button button;
        std::string label;
        std::function<void()> callback;
    };

    std::vector<std::vector<ButtonData>> globalButtonStack;

    void pushButton(std::vector<ButtonData> buttonData)
    {
        globalButtonStack.push_back(buttonData);
    }

    void popButton()
    {
        globalButtonStack.pop_back();
    }

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

    void fireButtonEventInMainLoop()
    {
        auto button = std::vector<ButtonData>();
        if (!globalButtonStack.empty())
        {
            button = globalButtonStack.back();
        }

        if (pressedButton == Button::UP)
        {
            upButtonEvent.dispatch();
            for (auto &buttonData : button)
            {
                if (buttonData.button == Button::UP)
                {
                    buttonData.callback();
                    break;
                }
            }
        }
        if (pressedButton == Button::DOWN)
        {
            downButtonEvent.dispatch();
            for (auto &buttonData : button)
            {
                if (buttonData.button == Button::DOWN)
                {
                    buttonData.callback();
                    break;
                }
            }
        }
        if (pressedButton == Button::CANCEL)
        {
            cancelButtonEvent.dispatch();
            for (auto &buttonData : button)
            {
                if (buttonData.button == Button::CANCEL)
                {
                    buttonData.callback();
                    break;
                }
            }
        }
        if (pressedButton == Button::CONFIRM)
        {
            confirmButtonEvent.dispatch();
            for (auto &buttonData : button)
            {
                if (buttonData.button == Button::CONFIRM)
                {
                    buttonData.callback();
                    break;
                }
            }
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
