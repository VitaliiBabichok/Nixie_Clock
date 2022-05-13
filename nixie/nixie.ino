#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "init.h"

void setup() {
  connectWifi();
  init_time_client();
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
