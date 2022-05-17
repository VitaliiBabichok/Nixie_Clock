#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"

int Hour;
int Minute;
int Second;
int set;

TaskHandle_t HandleSettingClok;

void setup() {
  connect_wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();
  get_time();
  
  xTaskCreate(task_increment_time, "updateTime", 1000, NULL, 1, NULL);
  xTaskCreate(task_settings_clock, "settingClock", 1000, NULL, 1, NULL);
}
void get_time() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Minute = timeClient.getMinutes();
  Second = timeClient.getSeconds();Hour = timeClient.getHours();
}
void task_increment_time(void *parameters) {
  for (;;)
  {
//    //For debug
//    Serial.print("Time: ");
//    Serial.print(Hour);
//    Serial.print(":");
//    Serial.print(Minute);
//    Serial.print(":");
//    Serial.println(Second);
    Second++;
    if (Second > MAX_SECOND)
    {
      Second = START_DIGIT;
      Minute++;
      if (Minute > MAX_MINUTE)
      {
        Minute = START_DIGIT;
        Hour++;
        if (Hour > MAX_HOUR)
        {
          Hour = START_DIGIT;
        }
      }
    }
    delay(1000);
  }
}

void task_settings_clock(void *parameters) //task
{
  for (;;)
  {
//    //This method works with interrupts on the comport
//    while (Serial.available() <= 0)
//    {
//    }
//    set = Serial.read() - '0';
    switch (set)
    {
      case 1:// set stopwatch

        Serial.println("=============================================");
        Serial.println("You have selected the stopwatch mode");
        break;
      case 2:// set timer

        Serial.println("=============================================");
        Serial.println("You have selected the timer mode");
        break;
      default:// set clock

        Serial.println("=============================================");
        Serial.println("Default mode clock");
        break;
    }
    vTaskSuspend(HandleSettingClok);
  }

}

void loop() {

}
