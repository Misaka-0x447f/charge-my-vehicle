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
    struct FloatConvertResult {
        float number;
        bool error;
    };
    // Utility function to try converting a string to a number. Returns FloatConvertResult indicating success or failure.
    lang::FloatConvertResult stringToFloat(const std::string& input) {
        std::istringstream iss(input);
        float number;
        iss >> number;
        if (!iss.fail() && iss.eof()) { // Check if conversion succeeded and reached the end of the string
            return {number, false}; // Return number with error flag as false
        }
        return {0.0f, true}; // Return 0.0 with error flag as true if conversion fails
    }
}
