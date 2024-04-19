#include "../.config.hpp"
#include "utils/request.hpp"
#include <ArduinoJson.h>
#include <string>
#include <functional>

namespace api
{
    std::string authorization = "";
    // 返回错误消息
    JsonDocument login(std::function<void(const std::string &)> errorMsgCb)
    {
        JsonDocument body = JsonDocument();
        body['userPhone'] = USER_NAME;
        body['userPassword'] = USER_PASSWORD_HASH;
        body['corpNo'] = USER_CORP_NO;
        body['deviceNo'] = USER_DEVICE_NO;
        body['loginType'] = "SZAPP";
        JsonDocument res = request::post(String('http://iot.shzhida.com:6771/api/login'), JsonDocument(), body);
        if (res.containsKey("error"))
        {
            errorMsgCb(res["error"].as<std::string>());
            return JsonDocument();
        }
        if (res.containsKey("data") && res["data"]["messge"] == "登录成功")
        {
            authorization = res["data"]["data"]["aliAccessToken"].as<std::string>();
            return res;
        }
        errorMsgCb(res["data"]["msg"]);
        return JsonDocument();
    }
}