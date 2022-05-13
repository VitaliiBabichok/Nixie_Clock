#define GMT 10800 // offset 3600 GMT+1

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* ssid     = "Ethernet";
const char* password = "WifiForChill";
