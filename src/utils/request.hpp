#pragma once
#include "../.config.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string>

namespace request
{
    JsonDocument post(String host, u16_t port, String path, JsonDocument header, JsonDocument body)
    {
        HTTPClient http;

        http.begin(host, port, path);

        // Add headers
        for (JsonPair headerPair : header.as<JsonObject>())
        {
            http.addHeader(headerPair.key().c_str(), headerPair.value().as<String>());
        }

        // application/json
        http.addHeader("Content-Type", "application/json");
        // user-agent
        http.addHeader("User-Agent", USERAGENT);

        // Convert body to string
        String bodyStr;
        serializeJson(body, bodyStr);

        int httpResponseCode = http.POST(bodyStr);

        JsonDocument jsonResponse;

        if (httpResponseCode <= 0)
            jsonResponse["error"] = "无法到达主机";
            return jsonResponse;
        if (http.getSize() <= 0)
            jsonResponse["error"] = "服务器的响应为空";
            return jsonResponse;
        String response = http.getString();
        DeserializationError error = deserializeJson(jsonResponse["data"], response);

        if (!error)
        {
            return jsonResponse;
        }

        jsonResponse["error"] = "解析 json 时遇到致命错误";
        return jsonResponse;
    }
}