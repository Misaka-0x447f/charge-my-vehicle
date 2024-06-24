#pragma once
#include <Arduino.h>
#include "../api.hpp"
#include <functional>

namespace component_charging_status
{
    int MaxTaskFreeableMemory = -1;
    TimerHandle_t xTimer = NULL;

    void worker()
    {
        api::ChargerStatus chargerStatus = api::getChargeStatus();
    }

    // 定时器回调函数
    void vTimerCallback(TimerHandle_t xTimer)
    {
        worker(); // 执行任务
        #ifdef DEBUG
        MaxTaskFreeableMemory = uxTaskGetStackHighWaterMark(NULL);
        #endif
        // 重新设置定时器以在任务完成后再次启动（单次定时器模式）
        xTimerChangePeriod(xTimer, pdMS_TO_TICKS(60000), 0);
    }

    void startWatching()
    {
        if (xTimer == NULL)
        {
            // 创建定时器，设置为单次定时器
            xTimer = xTimerCreate("Timer", pdMS_TO_TICKS(60000), pdFALSE, (void *)0, vTimerCallback);
            if (xTimer != NULL)
            {
                // 启动定时器
                xTimerStart(xTimer, 0);
            }
        }
        else
        {
            // 定时器已存在，直接启动
            xTimerStart(xTimer, 0);
        }
    }

    void stopWatching()
    {
        if (xTimer != NULL)
        {
            // 停止并删除定时器
            xTimerStop(xTimer, 0);
            xTimerDelete(xTimer, 0);
            xTimer = NULL;
        }
        view::statusBarCharging = "";
        view::statusCharging = "";
    }
} // namespace component_charging_status