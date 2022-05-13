#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"

int Hour;
int Minute;
int Second;

void setup() {
  connect_Wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();
  getTime();
  
  xTaskCreate(Task_incremet_time, "updateTime", 1000, NULL, 1, NULL);
}
void getTime() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Minute = timeClient.getMinutes();
  Second = timeClient.getSeconds();Hour = timeClient.getHours();
}
void Task_incremet_time(void *parameters) {
  for (;;)
  {
    Serial.print("Time: ");
    Serial.print(Hour);
    Serial.print(":");
    Serial.print(Minute);
    Serial.print(":");
    Serial.println(Second);
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

void loop() {

}
