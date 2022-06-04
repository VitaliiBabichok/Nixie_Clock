#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"

int Hour;
int Minute;
int Second;

TaskHandle_t HandleSetMode;

TimerHandle_t xTimer = NULL;

typedef enum {
  COMMAND_AUTO_TIME,
  COMMAND_SET_TIME,
  COMMAND_CLOCK,
  COMMAND_STOPWATCH,
  COMMAND_TIMER,
  COMMAND_ALARM,
  COMMAND_PAUSE,
  COMMAND_RESUME,
  COMMAND_RESET,
  COMMAND_ALARM_OFF,
  COMMAND_INVALID
} command_clock_t;

bool is_stopped = false;
bool is_alarm=false;
int32_t duration = 0;
command_clock_t prev_mod;

void setup() {
  connect_wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();

  xTaskCreate(task_set_mode, "selected mod clock", 1500, NULL, 1, &HandleSetMode);

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


void get_time() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Minute = timeClient.getMinutes();
  Second = timeClient.getSeconds();
  Hour = timeClient.getHours();
}
int get_time_range(int low_limit, int hight_limit, String str) 
{
  int Time;
  do {
    Serial.print(str);
    Serial.print(". Please enter digit in range ");
    Serial.print(low_limit);
    Serial.print(" to ");
    Serial.print(hight_limit);
    Serial.println(".");
    while (Serial.available() <= 0)
    {
    }
    Time = Serial.parseInt();
  }
  while ((Time < low_limit) || (Time > hight_limit));
  return Time;
}
int8_t get_minutes(int32_t count)
{
	return count/60;
}
int8_t get_seconds(int32_t count)
{
	return count%60;
}
int32_t get_timer_time(int mod)
{
	int32_t min;
	int32_t sec;
	set_time(&min, &sec, mod);
	return min * 60 + sec;

}
void set_time(int* Time1, int* Time2, int mod)
{
  switch (mod)
  {
    case COMMAND_SET_TIME:
      *Time1 = get_time_range(0, 24, "Set hour");
      *Time2 = get_time_range(0, 59, "Set minute");
      Second=0;
      break;

    case COMMAND_TIMER:
      *Time1 = get_time_range(0, 59, "Set minute");
      *Time2 = get_time_range(0, 59, "Set second");
      break;
  }
}


void check_time(uint8_t time1,uint8_t time2, uint8_t comp_time1, uint8_t comp_time2)
{
	if( (time1==comp_time1) && (time2==comp_time2) )
		{
			is_alarm = true;
		}
}

void buzzer(bool alarm)
{
	if(alarm)
	{
		Serial.println("Buzzer - ON!!!");
		digitalWrite(BUZZER,HIGH);
		delay(7000);
		digitalWrite(BUZZER,LOW);
		delay(3000);

	}
				
}


typedef void (*mode_function_t) (const int arg1, const int arg2);
mode_function_t mode_function = mode_clock;

void mode_clock(const int HH, const int MM) // format HH:MM
{
  parseIndicator(HH, MM);
}

void mode_stopwatch(const int MM, const int SS) // format MM:SS
{
  (void)MM;
  (void)SS;

  if (!is_stopped)
  {
    ++duration;
  }
 uint8_t minute = get_minutes(duration);
 uint8_t second = get_seconds(duration);

 parseIndicator(minute, second);
}

void mode_timer(const int MM, const int SS) // format MM:SS
{
  (void)MM;
  (void)SS;

  if (!is_stopped && (duration!=0) )
  {
    --duration;
  }

 uint8_t minute = get_minutes(duration);
 uint8_t second = get_seconds(duration);
check_time(minute,second,0,0);
buzzer(is_alarm);
parseIndicator(minute, second);
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
  mode_function(Hour, Minute);
}

// void print_settings()
// {
// 	Serial.print("Please enter digit in range 0 to 8\n");
// 	Serial.print("0 - Auto time (GMT +3),\n");
// 	Serial.print("1 - Set time,\n");
// 	Serial.print("2 - Clock,\n");
// 	Serial.print("3 - StopWatch,\n");
// 	Serial.print("4 - Timer,\n");
// 	Serial.print("5 - Stop time (Timer or StopWatch),\n");
// 	Serial.print("6 - Resume time (Timer or StopWatch),\n");
// 	Serial.print("7 - Reset time (Timer or StopWatch)\n");
// }

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

void parseIndicator(int Time1, int Time2)
{
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

void set_current_mode(int mod)
{
  switch (mod) {
    case COMMAND_AUTO_TIME:
      Serial.println("=============================================");
      Serial.println("Get time GMT +3");
			get_time();
      mode_function = mode_clock;
      break;

    case COMMAND_SET_TIME:
      Serial.println("=============================================");
      Serial.println("Set time manual");
      set_time(&Hour, &Minute,mod);
      mode_function = mode_clock;
      break;

    case COMMAND_CLOCK:
      Serial.println("=============================================");
      Serial.println("CLOCK");
      mode_function = mode_clock;
      break;

    case COMMAND_STOPWATCH:
      Serial.println("=============================================");
      Serial.println("STOPWATCH");
      is_stopped = false;
      duration = 0;
      mode_function = mode_stopwatch;
			prev_mod=COMMAND_STOPWATCH;
      break;

    case COMMAND_TIMER:
      Serial.println("=============================================");
      Serial.println("TIMER");
      is_stopped = false;
     	duration = get_timer_time(mod);
      mode_function = mode_timer;
			prev_mod=COMMAND_TIMER;
      break;

    case COMMAND_PAUSE:
      Serial.println("=============================================");
      Serial.println("PAUSE");
      is_stopped = true;
      break;

    case COMMAND_RESUME:
      Serial.println("=============================================");
      Serial.println("RESUME");
      is_stopped = false;
      break;

    case COMMAND_RESET:
      Serial.println("=============================================");
      Serial.println("RESET");
      duration=0;
      set_current_mode(prev_mod);
      break;

		case COMMAND_ALARM_OFF:
      Serial.println("=============================================");
      Serial.println("ALARM OFF");
      is_alarm=false;
      // mode_function = mode_clock;
			Serial.println("Buzzer - Of!!!");
			digitalWrite(BUZZER,LOW);
			set_current_mode(COMMAND_CLOCK);
      break;

    default:
      Serial.println("=============================================");
      Serial.println("Invalid command");
      Serial.println("Please enter digit in range 0 to 8");
      Serial.println("0 - Auto time (GMT +3),");
      Serial.println("1 - Set time,");
      Serial.println("2 - Clock,");
      Serial.println("3 - StopWatch,");
      Serial.println("4 - Timer,");
      Serial.println("5 - Alarm,");
      Serial.println("6 - Stop time (Timer or StopWatch),");
      Serial.println("7 - Resume time (Timer or StopWatch),");
      Serial.println("8 - Reset time (Timer or StopWatch),");
      Serial.println("9 - Alarm off");
      break;
  }

}

void task_set_mode(void *parameters)
{
  command_clock_t mod = COMMAND_AUTO_TIME;
	set_current_mode(mod);
  for (;;)
  {
    while (Serial.available() <= 0)
    {
    }

    mod = (command_clock_t)Serial.parseInt();
    set_current_mode(mod);
  }
}
