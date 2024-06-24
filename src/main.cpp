#include "utils/oled.hpp"
#include <Arduino.h>
#include "utils/wireless.hpp"
#include "./view.hpp"
#include <string>
#include "../.config.hpp"
#include "components/login.hpp"
#include "./components/chargingStatus.hpp"

#ifdef DEBUG
int UITaskFreeableMemory = -1;
#endif

void ui(void *pvParameters)
{
  while (1)
  {

    oled::drawLoop([]()
                   {
                     view::mainloop();
#ifdef DEBUG
                     UITaskFreeableMemory = uxTaskGetStackHighWaterMark(NULL);
#endif
                   });
    vTaskDelay(100 / portTICK_PERIOD_MS); // 添加任务延时以允许任务切换
  }
}

void worker(void *pvParameters)
{
  // 执行网络连接
  wireless::connect();
  // 等待网络连接完成
  xEventGroupWaitBits(wireless::s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
  component_login::login();
  component_charging_status::startWatching();
  vTaskDelay(100 / portTICK_PERIOD_MS); // 添加任务延时以允许任务切换
  view::errorCritical = "内部错误：worker 不应当结束。请重新启动。";
  printf("致命错误：Worker 不应当结束。请重新启动。");
  lang::infiniteSleep();
}

void printStackMemoryDebugInfo()
{
  printf("UI Task Freeable Memory: %d Bytes\n", UITaskFreeableMemory);
  printf("Worker Task Freeable Memory: %d Bytes\n", component_charging_status::MaxTaskFreeableMemory);
}

void setup(void)
{
  oled::init();
  TaskHandle_t uiTaskHandle;
  TaskHandle_t workerTaskHandle;
  //   // To check how many stack is needed: https://www.freertos.org/FAQMem.html#StackSize
  //   // allocate memory according to usage
  //   //                                                        unit: byte
  BaseType_t uiTaskCreateResult = xTaskCreate(ui, "UITask", 10 * 1000, NULL, 1, &uiTaskHandle);
  BaseType_t workerTaskCreateResult = xTaskCreate(worker, "WorkerTask", 10 * 1000, NULL, 1, &workerTaskHandle);

  //   // xTaskCreate函数在以下情况下可能会返回pdPASS以外的值：
  //   // 栈大小不足。
  //   // 堆内存不足，无法为新任务分配内存。
  //   // 任务优先级超出了系统允许的范围。
  if (uiTaskCreateResult != pdPASS)
  {
    printf("致命错误：UI 任务创建失败");
    view::errorCritical = "致命错误：UI 任务创建失败。请重新启动。";
    lang::infiniteSleep();
  }
  if (workerTaskCreateResult != pdPASS)
  {
    printf("致命错误：Worker 任务创建失败");
    view::errorCritical = "致命错误：Worker 任务创建失败。请重新启动。";
    lang::infiniteSleep();
  }
  xTimerStart(xTimerCreate("StackMemoryDebugTimer", 10000 / portTICK_PERIOD_MS, pdTRUE, NULL, [](TimerHandle_t xTimer)
                           { printStackMemoryDebugInfo(); }),
              0);
}

void loop(void)
{
  delay(1000);
}