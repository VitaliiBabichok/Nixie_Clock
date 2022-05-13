#define GMT 10800 // offset 3600 GMT+1

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* ssid     = "Ethernet";
const char* password = "WifiForChill";

//Connect to pins A, B, C & D of your SN74141 Nixie driver chip
//D is most significant bit
//A is least significant bit
// Highest hours need 2 bit (0-2), but for others mode (stopwatch, timer) need 3bits
#define A_H1 23
#define B_H1 22
#define C_H1 21
// #define D_H_h GND
// Lowest hours need 4 bit (0-9)
#define A_L2 19
#define B_L2 18
#define C_L2 5
#define D_L2 17
// Highest minutes need 3 bit (0-6)
#define A_H3 16
#define B_H3 4
#define C_H3 0
// #define D_H_m GND
// Lowest minutes need 4 bit (0-9)
#define A_L4 34
#define B_L4 35
#define C_L4 32
#define D_L4 33
