#include "define.h"

void connect_Wifi(){
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
}
void init_time_client() {
  Serial.println("Init time");
  // Initialize a NTPClient to get time
  timeClient.begin();
  Serial.println(".........");
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  timeClient.setTimeOffset(GMT);
  Serial.print("Time GMT +");
  Serial.println(GMT/3600);
}

void init_pin_indicator(){
  Serial.println("Init pin for indicator");
  Serial.println(".........");
  //First IN-12 display 0-5
  pinMode(A_H1, OUTPUT);
  pinMode(B_H1, OUTPUT);
  pinMode(C_H1, OUTPUT);
  //Second IN-12 display 0-9
  pinMode(A_L2, OUTPUT);
  pinMode(B_L2, OUTPUT);
  pinMode(C_L2, OUTPUT);
  pinMode(D_L2, OUTPUT);
  //Third IN-12 display 0-5
  pinMode(A_H3, OUTPUT);
  pinMode(B_H3, OUTPUT);
  pinMode(C_H3, OUTPUT);
  //Fourh IN-12 display 0-9
  pinMode(A_L4, OUTPUT);
  pinMode(B_L4, OUTPUT);
  pinMode(C_L4, OUTPUT);
  pinMode(D_L4, OUTPUT);
  Serial.println("Done.");
}

void init_led()
{
  Serial.print("Initializing LED...");
  pinMode(LED1, OUTPUT); //init pin  for LED1
  digitalWrite(LED1, LOW); //LED1 off
  pinMode(LED2, OUTPUT); //init pin  for LED2
  digitalWrite(LED2, LOW); //LED2 off
  Serial.println("  Done.");
}

void init_buzzer()
{
  Serial.print("Initializing Buzzer...");
  pinMode(Buzzer, OUTPUT); //init pin  for Buzzer
  digitalWrite(Buzzer, LOW); //Buzzer off
  Serial.println("  Done.");
}
