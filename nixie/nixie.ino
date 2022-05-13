#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"

void setup() {
  connect_Wifi();
  init_time_client();
  init_pin_indicator();
  init_led();
  init_buzzer();
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
