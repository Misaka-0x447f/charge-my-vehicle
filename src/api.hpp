#pragma once
#include "../.config.hpp"
#include "utils/request.hpp"
#include "./utils/lang.hpp"
#include <ArduinoJson.h>
#include <string>
#include <functional>
#include <iomanip>

namespace api
{
    std::string authorization = "";
    struct ChargerStatus
    {
        std::string text;
        std::string detailText;
        std::string code;
    };
    std::string getErrorMsg(JsonDocument res)
    {
        if (res.containsKey("status") && res["status"] != 200)
        {
            return res["msg"].as<std::string>();
        }
        if (res.containsKey("messge") && res["messge"] != "")
        {
            return res["messge"].as<std::string>();
        }
        if (res.containsKey("error") && res["error"] != "")
        {
            return res["error"].as<std::string>();
        }
        return "";
    };
    ChargerStatus getStatusText(std::string pileStatus, std::string pileStatusDetailCode, std::string alterText, std::string current, std::string voltage, std::string timeElapsed, std::string totalPower)
    {
        lang::FloatConvertResult currentFloat = lang::stringToFloat(current);
        lang::FloatConvertResult voltageFloat = lang::stringToFloat(voltage);
        lang::FloatConvertResult totalPowerFloat = lang::stringToFloat(totalPower);
        std::string powerConverted = "-- kW";
        if (currentFloat.error == false && voltageFloat.error == false)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << currentFloat.number * voltageFloat.number / 1000;
            powerConverted = ss.str() + "kW";
        }
        std::string totalPowerConverted = "-- kWh";
        if (totalPowerFloat.error == false)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << totalPowerFloat.number;
            totalPowerConverted = ss.str() + "kWh";
        }
        std::string detail = totalPowerConverted + " @ " + powerConverted;
        if (pileStatus == "01")
        {
            if (pileStatusDetailCode == "11")
            {
                return ChargerStatus({
                    .text = "正在充电" + timeElapsed,
                    .detailText = detail,
                    .code = "",
                });
            }
        }
        if (pileStatus == "02")
        {
            if (pileStatusDetailCode == "12")
            {
                return ChargerStatus({
                    .text = "已连接",
                    .detailText = "已预约",
                    .code = "",
                });
            }
        }
        if (pileStatus == "04")
        {
            if (pileStatusDetailCode == "14")
            {
                return ChargerStatus({
                    .text = "未连接",
                    .detailText = "",
                    .code = "",
                });
            }
            if (pileStatusDetailCode == "16")
            {
                return ChargerStatus({
                    .text = "已连接",
                    .detailText = "未启动",
                    .code = "",
                });
            }
            if (pileStatusDetailCode == "20")
            {
                return ChargerStatus({
                    .text = "充电中止" + timeElapsed,
                    .detailText = detail,
                    .code = "",
                });
            }
        }
        if (pileStatus == "06")
        {
            if (pileStatusDetailCode == "19")
            {
                return ChargerStatus({
                    .text = "充电完成" + timeElapsed,
                    .detailText = detail,
                    .code = "",
                });
            }
        }
        return ChargerStatus({
            .text = alterText,
            .detailText = detail,
            .code = "",
        });
    }
    JsonDocument login()
    {
        JsonDocument error = JsonDocument();
        JsonDocument body = JsonDocument();
        body["userPhone"] = USER_NAME;
        body["userPassword"] = USER_PASSWORD_HASH;
        body["corpNo"] = USER_CORP_NO;
        body["deviceNo"] = USER_DEVICE_NO;
        body["loginType"] = "SZAPP";
        JsonDocument res = request::post(API_HOST, 6771, "/api/login", JsonDocument(), body);
        std::string errorMsg = getErrorMsg(res);
        if (errorMsg != "")
        {
            error["error"] = errorMsg;
            return error;
        }
        if (res.containsKey("data") && res["data"]["messge"] == "登录成功")
        {
            authorization = res["data"]["data"]["aliAccessToken"].as<std::string>();
            return error;
        }
        error["error"] = "不可解析的数据格式";
        return error;
    }
    ChargerStatus getChargeStatus()
    {
        JsonDocument header = JsonDocument();
        header["Authorization"] = authorization;
        JsonDocument body = JsonDocument();
        body["userPhone"] = USER_NAME;
        body["corpNo"] = USER_CORP_NO;
        body["pileCode"] = USER_PILE_CODE;
        JsonDocument res = request::post(API_HOST, 6771, "/api/d1/app/custpile/pileInfo", header, body);
        std::string errorMsg = getErrorMsg(res);
        if (errorMsg != "")
        {
            return ChargerStatus({.text = "状态无效：" + errorMsg,
                                  .code = res["status"]});
        }
        std::string code = res["data"]["pileStatus"].as<std::string>() + res["data"]["pileStatusDetailCode"].as<std::string>();
        ChargerStatus sto = getStatusText(
            res["data"]["pileStatus"].as<std::string>(),
            res["data"]["pileStatusDetailCode"].as<std::string>(),
            "状态代码" + code,
            res["data"]["pileOrderAppDto"]["currentI"].as<std::string>(),
            res["data"]["pileOrderAppDto"]["currentU"].as<std::string>(),
            res["data"]["pileOrderAppDto"]["chargeHourMinute"].as<std::string>(),
            res["data"]["pileOrderAppDto"]["chargeVal"]
                .as<std::string>());
        sto.code = "0";
        return sto;
    }
}