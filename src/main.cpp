#include <Arduino.h>
#include "utils/buttonState.hpp"
#include "utils/oled.hpp"
#include "utils/wireless.hpp"
#include <string>
#include "../.config.hpp"

#ifdef DEBUG
int UITaskFreeableMemory = -1;
int WorkerTaskFreeableMemory = -1;
int NetworkTaskFreeableMemory = -1;
#endif

enum State
{
  RESETTING,
  CONNECTING,
  LOGGING_IN,
  HOME
};

State state = State::CONNECTING;

std::string getGlobalStatusMsg()
{
  if (!wireless::status.empty())
    return wireless::status;
  return "待机";
}

void ui(void *pvParameters)
{
  while (1)
  {
    // 任务：绘制 GUI
    oled::drawLoop([]()
                   {
                     oled::u8g2.drawUTF8(0, 0, lang::toString(getGlobalStatusMsg()));
                     // 绘制全局状态
                     oled::u8g2.drawUTF8(0, 0, lang::toString(getGlobalStatusMsg()));
                     oled::u8g2.drawHLine(0, 13, oled::u8g2.getDisplayWidth());
                     // 绘制按钮
                     if (!buttonState::globalButtonStack.empty())
                     {
                       oled::drawMenu(buttonState::globalButtonStack.back());
                     }
                                          oled::u8g2.drawHLine(0, oled::u8g2.getDisplayHeight() - 13, oled::u8g2.getDisplayWidth());
#ifdef DEBUG
                     UITaskFreeableMemory = uxTaskGetStackHighWaterMark(NULL);
#endif
                     vTaskDelay(100 / portTICK_PERIOD_MS); // 添加任务延时以允许任务切换
                   });
  }
}

void worker(void *pvParameters)
{
  // 任务：执行逻辑
  // 如果网络失败，等待重新启动完成
  while (1)
  {
    if (wireless::failed)
    {
      state = State::RESETTING;
      return;
    }
#ifdef DEBUG
    WorkerTaskFreeableMemory = uxTaskGetStackHighWaterMark(NULL);
#endif
    vTaskDelay(100 / portTICK_PERIOD_MS); // 添加任务延时以允许任务切换
  }
}

void printStackMemoryDebugInfo()
{
  printf("UI Task Freeable Memory: %d Bytes\n", UITaskFreeableMemory);
  printf("Worker Task Freeable Memory: %d Bytes\n", WorkerTaskFreeableMemory);
  printf("Network Task Freeable Memory: %d Bytes\n", NetworkTaskFreeableMemory);
}

void setup(void)
{
  buttonState::buttonStateListenerInit();
  oled::init();
  TaskHandle_t uiTaskHandle;
  TaskHandle_t workerTaskHandle;
  // To check how many stack is needed: https://www.freertos.org/FAQMem.html#StackSize
  // allocate memory according to usage
  //                         unit: byte
  BaseType_t uiTaskCreateResult = xTaskCreate(ui, "UITask", 10 * 1000, NULL, 1, &uiTaskHandle);
  BaseType_t workerTaskCreateResult = xTaskCreate(worker, "WorkerTask", 10 * 1000, NULL, 1, &workerTaskHandle);
  // 创建一个执行网络连接的一次性任务
  BaseType_t networkTaskCreateResult = xTaskCreate([](void *parameters)
                                                   {
      // 执行网络连接
      wireless::connect();
#ifdef DEBUG
      NetworkTaskFreeableMemory = uxTaskGetStackHighWaterMark(NULL);
#endif
      // 删除任务
      vTaskDelete(NULL); },
                                                   "NetworkTask", 2 * 1000, NULL, 1, NULL);

  // xTaskCreate函数在以下情况下可能会返回pdPASS以外的值：
  // 栈大小不足。
  // 堆内存不足，无法为新任务分配内存。
  // 任务优先级超出了系统允许的范围。
  if (uiTaskCreateResult != pdPASS)
  {
    printf("致命错误：UI 任务创建失败");
    oled::drawGlobalFatalMsg("致命错误：UI 任务创建失败。请重新启动。");
    lang::infiniteSleep();
  }
  if (workerTaskCreateResult != pdPASS)
  {
    printf("致命错误：Worker 任务创建失败");
    oled::drawGlobalFatalMsg("致命错误：Worker 任务创建失败。请重新启动。");
    lang::infiniteSleep();
  }
  if (networkTaskCreateResult != pdPASS)
  {
    printf("致命错误：Network 任务创建失败");
    oled::drawGlobalFatalMsg("致命错误：Network 任务创建失败。请重新启动。");
    lang::infiniteSleep();
  }
  xTimerStart(xTimerCreate("StackMemoryDebugTimer", 10000 / portTICK_PERIOD_MS, pdTRUE, NULL, [](TimerHandle_t xTimer)
                           { printStackMemoryDebugInfo(); }),
              0);
}

void loop(void)
{
  buttonState::fireButtonEventInMainLoop();
  delay(100);
}