#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"


TaskHandle_t HandleSettingClock;
TaskHandle_t HandleClock;
TaskHandle_t HandleSetMode;

typedef enum command_clock_t {
  COMMAND_CLOCK,
  COMMAND_SET_TIME,
  COMMAND_STOPWATCH,
  COMMAND_TIMER,
  COMMAND_INVALID
};
command_clock_t Set = COMMAND_CLOCK;

TimerHandle_t xTimer = NULL;

int Hour;
int Minute;
int Second=50;


void setup() {
  connect_wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();
  get_time();

  xTimer = xTimerCreate( "Timer", 1000, pdTRUE, NULL, increment_time );
  if ( xTimer == NULL )
  {
    Serial.println("Timer was not created");
  }
  else
  {
    if ( xTimerStart( xTimer, 0 ) != pdPASS )
    {
      Serial.println("Timer was not started");
    }
  }


}

void increment_time(TimerHandle_t xTimer) {

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
}


void get_time() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Minute = timeClient.getMinutes();
  Second = timeClient.getSeconds();
  Hour = timeClient.getHours();
}
void loop() {


}
