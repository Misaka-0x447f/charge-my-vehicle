#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <map>
#include <list>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <string>
#include "./lang.hpp"
#include "./type.hpp"

class EventHandler
{
public:
    struct TaskParams
    {
        CallbackType callback;
        EventGroupHandle_t eventGroup;
    };

    EventHandler()
    {
        eventGroup = xEventGroupCreate();
    }

    ~EventHandler()
    {
        vEventGroupDelete(eventGroup);
    }

    static void taskFunction(void *params)
    {
        auto taskParams = static_cast<TaskParams *>(params);
        while (true)
        {
            xEventGroupWaitBits(taskParams->eventGroup, BIT0, pdTRUE, pdFALSE, portMAX_DELAY);
            taskParams->callback();
        }
    }

    std::function<void()> subscribe(CallbackType callback)
    {
        std::string taskName = "EventHandlerTask" + std::to_string(rand() + time(0));
        auto taskParams = new TaskParams{callback, eventGroup};

        TaskHandle_t taskHandle;

        xTaskCreate(&EventHandler::taskFunction, taskName.c_str(), 2048, taskParams, 1, &taskHandle);

        return [this, taskName, taskHandle]()
        {
            vTaskDelete(taskHandle);
        };
    }

    void dispatch()
    {
        xEventGroupSetBits(eventGroup, BIT0);
    }

private:
    EventGroupHandle_t eventGroup;
};