#include <Arduino.h>
#include "utils/oled.hpp"
#include "utils/buttonState.hpp"
#include "utils/wireless.hpp"
#include <string>
#include "../.config.hpp"

void setup(void) {
  buttonState::buttonStateListenerInit();
  wireless::connect();
  oled::init();
}

std::string drawGlobalStatusMsg() {
  std::string wirelessStatus = wireless::status;
  if (!wirelessStatus.empty())
    return wirelessStatus;
  return "待机";
}

void loop(void) {
  oled::drawLoop([]() {
    oled::u8g2.drawUTF8(0, 0, lang::toString(drawGlobalStatusMsg()));
    oled::u8g2.drawUTF8(0, 50, lang::toString(buttonState::getButtonName(buttonState::pressedButton)));
  });
  // delay between each page
  delay(100);
}