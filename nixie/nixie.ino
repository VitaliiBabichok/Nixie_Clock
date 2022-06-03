#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"

int Hour;
int Minute;
int Second;

int Minute_Start;
int Second_Start;

TaskHandle_t HandleSettingClock;
TaskHandle_t HandleClock;
TaskHandle_t HandleSetMode;

TimerHandle_t xTimer = NULL;

typedef enum command_clock_t {
  COMMAND_CLOCK,
  COMMAND_STOPWATCH,
  COMMAND_TIMER,
  COMMAND_INVALID
};
command_clock_t Set = COMMAND_CLOCK;

typedef void (*general_function) (const int arg1, const int arg2);

void set_clock(const int HH, const int MM) //format HH:MM
{
  parseIndicator(HH, MM);
}

void set_stopwatch(const int MM, const int SS) //format MM:SS
{
  //func stop time;

  //func reset time;

  //func exit mode;
  parseIndicator(MM - Minute_Start, SS - Second_Start);
}

void set_timer(const int MM, const int SS) //format MM:SS
{
  //func stop time;

  //func reset time;

  //func check time set and real time
  //func buzzer

  //func exit mode;
  parseIndicator(Minute_Start - MM, Second_Start - SS);
}

void set_alarm(const int HH, const int MM) //format HH:MM
{
  //func check time set and real time
  //func buzzer

  // func exit mode
}

void show_mode (general_function f, const int Time1, const int Time2)
{
  f(Time1, Time2) ;   // call the passed-in function
}



void setup() {
  connect_wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();
  get_time();

  xTaskCreate(task_set_mode,"selected mod clock", 1500, NULL, 1, NULL);
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
  current_mode();

}

void task_set_mode(void *parameters) {
  for (;;)
  {
      while (Serial.available() <= 0)
      {
      }
      Set = (command_clock_t)Serial.parseInt();
  }
}
void current_mode()
{
  switch (Set) {
    case COMMAND_CLOCK:
      Serial.println("=============================================");
      Serial.println("CLOCK");
      show_mode(set_clock, Hour, Minute);
      break;

    case COMMAND_STOPWATCH:
      Serial.println("=============================================");
      Serial.println("STOPWATCH");
      show_mode(set_stopwatch, Minute, Second);
      break;

    case COMMAND_TIMER:
      Serial.println("=============================================");
      Serial.println("TIMER");
      show_mode(set_timer, Minute, Second);
      break;

    default:
      Serial.println("=============================================");
      Serial.println("Invalid command");
      Serial.println("Please enter digit in range 0 to 2");
      Serial.println("0 - Clock,");
      Serial.println("1 - StopWatch,");
      Serial.println("2 - Timer");
      break;

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
void loop() {


}
