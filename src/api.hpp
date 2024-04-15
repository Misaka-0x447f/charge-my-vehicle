#include "../.config.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>

namespace api
{
    JsonDocument sendPostRequest(String url, JsonDocument header, JsonDocument body)
    {
        HTTPClient http;

        http.begin(url);

        // Add headers
        for (JsonPair headerPair : header.as<JsonObject>())
        {
            http.addHeader(headerPair.key().c_str(), headerPair.value().as<String>());
        }

        // Convert body to string
        String bodyStr;
        serializeJson(body, bodyStr);

        int httpResponseCode = http.POST(bodyStr);

        JsonDocument jsonResponse;

        if (httpResponseCode <= 0)
            return jsonResponse;
        if (http.getSize() <= 0)
            return jsonResponse;
        String response = http.getString();
        DeserializationError error = deserializeJson(jsonResponse, response);

        if (!error)
        {
            return jsonResponse;
        }

        return jsonResponse; // empty json object
    }
}