#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "FS.h"

/*
    Code to connect to WPA Enterprise "eduroam"
*/
extern "C"
{
    #include "user_interface.h"
    #include "wpa2_enterprise.h"
}

char ssid[32] = "eduroam";
char password[64] = {0x00};

static u8 ent_username[] = "YOUR USER NAME";
static u8 ent_password[] = "YOUR PASSWORD";


/*
    Code for connection to the HTTP Server.
*/
char* host = "api.github.com";
char* route = "/";
int port = 443;    // Unused, here for backward compatibility

/*
    Definitions to establish secure connection and set Serial timeout.
*/
#define ESPTIMEOUT 4000
#define SECURE_CONNECTION

File ca;
File key;

/*
    NTP Client to syncronize time with NTP Server.
*/
char* ntpserver = "ntp.ufsc.br";
NTPClient * timeClient;
WiFiUDP ntpUDP;
