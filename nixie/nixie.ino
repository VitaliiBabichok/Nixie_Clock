#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"


TaskHandle_t HandleSettingClock;
TaskHandle_t HandleClock;

typedef enum command_clock_t {
  COMMAND_CLOCK,
  COMMAND_SET_TIME,
  COMMAND_STOPWATCH,
  COMMAND_TIMER,
  COMMAND_INVALID
};

command_clock_t Set = COMMAND_CLOCK;

int Hour;
int Minute;
int Second;


void setup() {
  connect_wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();
  get_time();

  xTaskCreate(task_increment_time, "update Time", 1000, NULL, 1, NULL);
  xTaskCreate(task_settings_clock, "setting Clock", 1000, NULL, 1, &HandleSettingClock);
  xTaskCreate(task_clock, "Clock", 1000, NULL, 1, &HandleClock);
  xTaskCreate(set_mode, "set mode", 1500, NULL, 1, NULL);
}
void get_time() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Minute = timeClient.getMinutes();
  Second = timeClient.getSeconds(); Hour = timeClient.getHours();
}

void set_time()
{
  // set hour
  do {
    Serial.println("Set hours. Please enter digit in range 0 to 24.");
    while (Serial.available() <= 0)
    {
    }
    Hour = Serial.parseInt();
  }
  while ((Hour < START_DIGIT) || (Hour > MAX_HOUR));

  // set minute
  do {
    Serial.println("Set minutes. Please enter digit in range 0 to 59.");
    while (Serial.available() <= 0)
    {
    }
    Minute = Serial.parseInt();
  }
  while ((Minute < START_DIGIT) || (Minute > MAX_MINUTE));
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

void task_settings_clock(void *parameters) {
  for (;;)
  {
    //    //This method works with interrupts on the comport
    //    while (Serial.available() <= 0)
    //    {
    //    }
    //    set = Serial.read() - '0';
    switch (Set)
    {
      case COMMAND_CLOCK:// selected set time
        Serial.println("=============================================");
        Serial.println("You have selected the clock mode");
        break;
        
      case COMMAND_SET_TIME:// selected set time
        Serial.println("=============================================");
        Serial.println("You have selected the set time mode");
        set_time();// set Hour and Minute manual
        vTaskResume(HandleClock);
        break;

      case COMMAND_STOPWATCH:// selected stopwatch

        Serial.println("=============================================");
        Serial.println("You have selected the stopwatch mode");
        break;
      case COMMAND_TIMER:// selected timer

        Serial.println("=============================================");
        Serial.println("You have selected the timer mode");
        break;
      default:// invalid command

        Serial.println("=============================================");
        Serial.println("Invalid command");
        break;
    }
    vTaskSuspend(HandleSettingClock);
  }
}

void  nixieWriteIndicator(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t value) {
  if (d == NULL) // Digit range 0 to 5
  {
    //    digitalWrite(c, (value & 0x04) >> 2);
    //    digitalWrite(b, (value & 0x02) >> 1);
    //    digitalWrite(a, value & 0x01);
    Serial.println("C B A");
    Serial.println(String((value & 0x04) >> 2) + " " + String((value & 0x02) >> 1) + " " + String(value & 0x01));
  }
  else // Digit range 0 to 9
  {
    //    digitalWrite(d, (value & 0x08) >> 3);
    //    digitalWrite(c, (value & 0x04) >> 2);
    //    digitalWrite(b, (value & 0x02) >> 1);
    //    digitalWrite(a, value & 0x01);
    Serial.println("D C B A");
    Serial.println(String((value & 0x08) >> 3) + " " + String((value & 0x04) >> 2) + " " + String((value & 0x02) >> 1) + " " + String(value & 0x01));
  }

}

void parseIndicator(int Time1, int Time2) {

  Serial.println(Time1);
  int Time1_H = Time1 / 10;
  nixieWriteIndicator(A_H1, B_H1, C_H1, NULL , Time1_H);
  int Time1_L = Time1 % 10;
  nixieWriteIndicator(A_L2, B_L2, C_L2, D_L2, Time1_L);

  Serial.println(Time2);
  int Time2_H = Time2 / 10;
  nixieWriteIndicator(A_H3, B_H3, C_H3, NULL, Time2_H);
  int Time2_L = Time2 % 10;
  nixieWriteIndicator(A_L4, B_L4, C_L4, D_L4, Time2_L);
}



void task_clock(void *parameters) {
  for (;;)
  {
    parseIndicator(Hour, Minute);
    delay(1000);
  }
}

void set_mode(void *parameters) {
  for (;;)
  {
    do {
      Serial.println("Please enter digit in range 0 to 3.");
      while (Serial.available() <= 0)
      {
      }
      Set = (command_clock_t)Serial.parseInt();
    } while ((Set < COMMAND_SET_TIME) || (Set > COMMAND_TIMER));

    vTaskSuspend(HandleClock);
    vTaskResume(HandleSettingClock);
    vTaskDelay(1000);
  }
}

void loop() {

}
