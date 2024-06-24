#pragma once

#include "../api.hpp"
#include "../utils/oled.hpp"
#include "../utils/lang.hpp"
#include "../view.hpp"

namespace component_login
{
    void login()
    {
        JsonDocument res = api::login();
        if (res.isNull())
            return;
        if (res.containsKey("error"))
        {
            std::string errorMsg = res["error"].as<std::string>();
            printf("登录失败, 原因: %d\n，等待重新启动", errorMsg.c_str());
            view::statusBarLogin = "登录失败";
            view::errorCritical = "登录失败: " + errorMsg + "; 等待重新启动";
            // Wait for 30 seconds
            vTaskDelay(30000 / portTICK_PERIOD_MS);
            // Restart the device
            esp_restart();
        }
    }
}