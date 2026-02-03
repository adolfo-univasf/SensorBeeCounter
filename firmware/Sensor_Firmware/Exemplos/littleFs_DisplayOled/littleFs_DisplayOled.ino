/*
**********************   Para não ter erro de compilação   ********************** 

Boards Manager:
-> Esp32 by Espressif Versão 2.0.17

Library:
->Heltec ESP32 Dev-Boards by Heltec Automation

*/

#include "heltec.h"
#include <LittleFS.h>
#include <Arduino.h>

void setup() {
  Serial.begin(115200);

  Heltec.begin(true, false, false, false, 915E6);

  if (!LittleFS.begin(true)) {
    Heltec.display->drawString(0, 0, "LittleFS ERRO");
  } else {
    Heltec.display->drawString(0, 0, "LittleFS OK");
  }
  //Heltec.display->display();
}

void loop() {
}
