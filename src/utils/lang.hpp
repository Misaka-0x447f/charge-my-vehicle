#pragma once

#include <sstream>
#include <string>
#include "esp_sleep.h"
#include <functional>
#include "freertos/FreeRTOS.h"
#include "./type.hpp"

namespace lang
{
    template <typename T>
    const char *toString(const T &value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str().c_str();
    }
    void infiniteSleep()
    {
        while (1)
        {
            esp_deep_sleep_start();
        }
    }
}
