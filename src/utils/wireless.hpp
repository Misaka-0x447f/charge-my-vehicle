#include "../../.config.hpp"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include <string>

#define WIFI_CONNECTING_BIT BIT0
#define WIFI_CONNECTED_BIT BIT1
#define WIFI_FAIL_BIT BIT2
#define WIFI_CONNECTED_NO_IP_BIT BIT3

namespace wireless
{
    std::string status = "";
    boolean failed = false;

    EventGroupHandle_t s_wifi_event_group;

    void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTING_BIT);
            printf("正在连接\n");
            status = "正在连接";
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);

            wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
            printf("连接断开，SSID: %s, 原因: %d\n，等待重新启动", WIFI_SSID, event->reason);
            status = "网络失败";
            failed = true;
            // Wait for 60 seconds
            vTaskDelay(60000 / portTICK_PERIOD_MS);
            // Restart the device
            esp_restart();
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_NO_IP_BIT);
            printf("正在获取 IP 地址\n");
            status = "正在获取 IP 地址";
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
            printf("已连接\n");
            status = "";
            buttonState::pushButton({
                {button : buttonState::Button::UP, label : "", callback : []()
                 {
                     printf("上\n");
                 }},
                    {
                        button : buttonState::Button::DOWN,
                        label : "下",
                        callback : []()
                        {
                            printf("下\n");
                        }
                    },
                    {
                        button : buttonState::Button::CANCEL,
                        label : "取消",
                        callback : []()
                        {
                            printf("取消\n");
                        }
                    },
                {
                button:
                    buttonState::Button::CONFIRM,
                        label : "好",
                                callback : []()
                    {
                        printf("好\n");
                    }
                }});
                }
    }

    void connect()
    {
                s_wifi_event_group = xEventGroupCreate();

                tcpip_adapter_init();
                ESP_ERROR_CHECK(esp_event_loop_create_default());

                wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
                ESP_ERROR_CHECK(esp_wifi_init(&cfg));

                ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
                ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

                wifi_config_t wifi_config;
                memset(&wifi_config, 0, sizeof(wifi_config));
                memcpy(wifi_config.sta.ssid, WIFI_SSID, sizeof(WIFI_SSID));
                memcpy(wifi_config.sta.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));

                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                ESP_ERROR_CHECK(esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &wifi_config));
                ESP_ERROR_CHECK(esp_wifi_start());
    }
        }
