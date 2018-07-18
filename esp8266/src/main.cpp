
#include "wifidefs.h"
#define LEGACY

enum {ESPBUSY, ESPIDLE} machine_state;
enum {CLOCKSYNCED, CLOCKNOTSYNCED} clock_state;
enum {ESPDISCONNECTED, ESPCONNECTED} connection_state;
enum {CLIENTCONNECTED, CLIENTDISCONNECTED} client_state;
enum {ESPMOUNTED, ESPNOTMOUNTED} fs_state;

void connect();
void hello_host();
void load_files(WiFiClientSecure * client);
void incoming_command(String received_data);
void sync_clock();

unsigned long int last_time_recordered = 0;
unsigned long int last_response_time = 0;

void setup()
{
    machine_state = ESPBUSY;
    Serial.begin(115200);

    if (!SPIFFS.begin())
        fs_state = ESPNOTMOUNTED;
    else
    {
        fs_state = ESPMOUNTED;
        #ifdef SECURE_CONNECTION
            ca = SPIFFS.open("/cert.der", "r");
            key = SPIFFS.open("/key.der", "r");
        #endif
    }

    connect();

    //load_files(&client);

    if (connection_state != ESPCONNECTED)
        Serial.print("ERROR=CONNECTIONFAIL\r");

    if (fs_state != ESPMOUNTED)
        Serial.print("ERROR=FILESYSTEMFAIL\r");

    if (fs_state == ESPMOUNTED && connection_state == ESPCONNECTED)
        Serial.print("READY\r");

    client_state = CLIENTDISCONNECTED;
    //hello_host();

    timeClient = new NTPClient(ntpUDP, ntpserver, 0, 12*60*60*1000);
    clock_state = CLOCKNOTSYNCED;

    machine_state = ESPIDLE;
}


bool response_received = false;
bool first_line = false;

void loop ()
{
    if (Serial.available() > 0 && machine_state == ESPIDLE)
    {
        machine_state = ESPBUSY;

        String received = Serial.readStringUntil('\r');
        incoming_command(received);
        machine_state = ESPIDLE;
    }
}

String data;
String command = "";
String parameter = "";
String payload = "";
int delimiterPosition = -1;

String msg_size = "";
bool gettingSize = false;

void incoming_command (String received_data)
{

    String incoming = received_data.substring(0,2);

    int received_length = received_data.length();

    if (incoming != "AT")
    {
        Serial.print("ERROR=INVALIDCOMMAND\r");
        return;
    }

    data = received_data.substring(2, received_length);

    for (int i = 0; i < data.length(); i++)
    {
        if (data[i] == '!')
        {
            gettingSize = !gettingSize;
        }
        else if (gettingSize)
        {
            if (data[i] == '=')
            {
                delimiterPosition = i;
                gettingSize = !gettingSize;
            }
            else
                msg_size += data[i];
        }
        else if (data[i] != '=' && delimiterPosition == -1)
        {
            command += data[i];
        }
        else if (data[i] == '=' && delimiterPosition == -1)
        {
            delimiterPosition = i;
        }
        else if (data[i] != ' ')
        {
            parameter += data[i];
        }
    }

    int parameter_length = parameter.length();

    if (command == "+SYSTEMCHECK")
    {
        // This shouldn't be used in the epos, only for testing
        Serial.print("CONNECTION:");
        (connection_state == ESPCONNECTED) ? Serial.print("OK") : Serial.print("ERROR");
        Serial.print("&FILESYSTEM:");
        (fs_state == ESPMOUNTED) ? Serial.print("OK") : Serial.print("ERROR");
        Serial.print("\r");

    }
    else if (command == "+SYSTEMREADY")
    {

        (connection_state == ESPCONNECTED && fs_state == ESPMOUNTED) ?
            Serial.print("OK\r") : Serial.print("ERROR\r");

    }
    else if (command == "+RESPONSETIME")
    {

        Serial.print("OK=");
        Serial.print(last_response_time);
        Serial.print('\r');

    }
    else if (command == "+GETHOST")
    {

        Serial.print("OK=");
        Serial.print(host);
        Serial.print("\r");

    }
    else if (command == "+GETROUTE")
    {

        Serial.print("OK=");
        Serial.print(route);
        Serial.print("\r");

    }
    else if (command == "+GETPORT")
    {

        Serial.print("OK=");
        Serial.print(port);
        Serial.print("\r");

    }
    else if (command == "+SETHOST")
    {

        host = new char[parameter_length + 1];
        parameter.toCharArray(host, parameter_length + 1);
        Serial.print("OK\r");

    }
    else if (command == "+SETROUTE")
    {

        route = new char[parameter_length + 1];
        parameter.toCharArray(route, parameter_length + 1);
        Serial.print("OK\r");

    }
    else if (command == "+SETPORT")
    {

        port = parameter.toInt();
        Serial.print("OK\r");

    }
    else if (command == "+CONNECTWIFI")
    {

        connect();
        (connection_state == ESPCONNECTED) ? Serial.print("OK\r") : Serial.print("ERROR\r");

    }
    else if (command == "+SYNCCLOCK")
    {
        sync_clock();
        (clock_state == CLOCKSYNCED) ? Serial.print("OK\r") : Serial.print("ERROR=TIMEOUT\r");

    }
    else if (command == "+TIMESTAMP")
    {
        if (connection_state == ESPCONNECTED && clock_state == CLOCKNOTSYNCED)
            sync_clock();

        if (clock_state == CLOCKSYNCED)
        {
            Serial.print("OK=");
            Serial.print(timeClient->getEpochTime());
            Serial.print("\r");
        }
        else
          Serial.print("ERROR=CLOCKNOTSYNCED\r");

    }
    else if (command == "+FTIMESTAMP")
    {
        if (connection_state == ESPCONNECTED && clock_state == CLOCKNOTSYNCED)
            sync_clock();

        if (clock_state == CLOCKSYNCED)
        {
            Serial.print("OK=");
            Serial.print(timeClient->getFullFormattedTime());
            Serial.print("\r");
        }
        else
            Serial.print("ERROR=CLOCKNOTSYNCED\r");

    }
    else if(command == "+HEAPSIZE")
    {

        Serial.printf("OK=%u\r", ESP.getFreeHeap());

    }
    else if (command == "+GET")
    {
        // TODO: Handle secure connections. Apparently, there is a bug on the
        // client.connect for some certificates like pastebin.com
        last_time_recordered = millis();
        HTTPClient http;
        http.setTimeout(1000);
        if (parameter.length() > 0)
        {
            // Select https if not especified
            if (!parameter.startsWith("http://"))
                parameter = "http://" + parameter;
        }
        else
            parameter = "http://" + String(host) + String(route);

        http.begin(parameter);

        int status_code = http.GET();
        if(status_code == HTTP_CODE_OK) {
            Serial.print( "OK=" + http.getString() + "\r" );
            last_response_time = millis() - last_time_recordered;
        }
        else
            Serial.print("ERROR=HTTP" + String(status_code) + "\r");

        http.~HTTPClient();
    }
    else if (command == "+GETS")
    {
        // TODO: Handle secure connections. Apparently, there is a bug on the
        // client.connect for some certificates like pastebin.com
        last_time_recordered = millis();
        HTTPClient http;
        http.setTimeout(1000);
        if (parameter.length() > 0)
        {
            // Select https if not especified
            if (!parameter.startsWith("https://"))
                parameter = "https://" + parameter;
        }
        else
            parameter = "https://" + String(host) + String(route);

        http.begin(parameter, "");

        int status_code = http.GET();
        if(status_code == HTTP_CODE_OK) {
            Serial.print( "OK=" + http.getString() + "\r" );
            last_response_time = millis() - last_time_recordered;
        }
        else
            Serial.print("ERROR=HTTP" + String(status_code) + "\r");

        http.~HTTPClient();
    }
    else if (command == "+PAYLOAD")
    {

        payload = parameter;
        Serial.print("OK\r");

    }
    else if (command == "+GETPAYLOAD")
    {

        Serial.print("OK=" + payload + "\r");

    }
    else if (command == "+POST")
    {
        // TODO: Handle secure connections. Apparently, there is a bug on the
        // client.connect for some certificates like pastebin.com
        last_time_recordered = millis();
        HTTPClient http;
        http.setTimeout(1000);
        if (parameter.length() > 0)
        {
            // Select https if not especified
            if (!parameter.startsWith("http://"))
                parameter = "http://" + parameter;
        }
        else
            parameter = "http://" + String(host) + String(route);

        http.begin(parameter);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int status_code = http.POST(payload);
        if(status_code == HTTP_CODE_OK ||
           status_code == HTTP_CODE_CREATED ||
           status_code == HTTP_CODE_ACCEPTED)
        {
            Serial.print( "OK=" + http.getString() + "\r" );
            last_response_time = millis() - last_time_recordered;
            payload = "";
        }
        else
            Serial.print("ERROR=HTTP" + String(status_code) + "\r");

        http.~HTTPClient();
    }
    else if (command == "+POSTS")
    {
        // TODO: Handle secure connections. Apparently, there is a bug on the
        // client.connect for some certificates like pastebin.com
        last_time_recordered = millis();
        HTTPClient http;
        http.setTimeout(1000);
        if (parameter.length() > 0)
        {
            // Select https if not especified
            if (!parameter.startsWith("https://"))
                parameter = "https://" + parameter;
        }
        else
            parameter = "https://" + String(host) + String(route);

        http.begin(parameter, "");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int status_code = http.POST(payload);
        if(status_code == HTTP_CODE_OK ||
           status_code == HTTP_CODE_CREATED ||
           status_code == HTTP_CODE_ACCEPTED)
        {
            Serial.print( "OK=" + http.getString() + "\r" );
            last_response_time = millis() - last_time_recordered;
            payload = "";
        }
        else
            Serial.print("ERROR=HTTP" + String(status_code) + "\r");

        http.~HTTPClient();
    }
    else if (command == "+IP")
    {
        if (connection_state == ESPCONNECTED)
        {
            ip_info info;
            wifi_get_ip_info(STATION_IF, &info);
            Serial.print("OK=");
            Serial.print(info.ip.addr);
            Serial.print("\r");
        }
        else
            Serial.print("ERROR=ESPDISCONNECTED\r");

    }
    else if (command == "+FIP")
    {
        if (connection_state == ESPCONNECTED)
        {
            unsigned char bytes[4];
            ip_info info;
            wifi_get_ip_info(STATION_IF, &info);

            bytes[0] = info.ip.addr         & 0xFF;
            bytes[1] = (info.ip.addr >> 8)  & 0xFF;
            bytes[2] = (info.ip.addr >> 16) & 0xFF;
            bytes[3] = (info.ip.addr >> 24) & 0xFF;

            Serial.print("OK=");
            for (int i = 0; i < 4; i++)
            {
                Serial.print(bytes[i]);
                i == 3 ? Serial.print("\r") : Serial.print(".");
            }
        }
        else
            Serial.print("ERROR=ESPDISCONNECTED\r");

    }
    else if (command == "+RSSI")
    {
            sint8_t rssi = wifi_station_get_rssi();
            Serial.print("OK=");
            Serial.print(rssi);
            Serial.print("\r");
    }
    else if (command == "+CHANNEL")
    {
        uint8_t channel = wifi_get_channel();
        Serial.print("OK=");
        Serial.print(channel);
        Serial.print("\r");
    }
    else if (command == "+RESTART")
    {

        ESP.restart();

    }
    else
    {

        Serial.print("ERROR=UNKNOWNCOMMAND\r");

    }

    delimiterPosition = -1;
    parameter = "";
    data = "";
    command = "";
    msg_size = "";
    gettingSize = false;
}

wl_status_t wifi_status ()
{
    station_status_t status = wifi_station_get_connect_status();

    switch(status)
    {
        case STATION_GOT_IP:
            return WL_CONNECTED;
        case STATION_NO_AP_FOUND:
            return WL_NO_SSID_AVAIL;
        case STATION_CONNECT_FAIL:
        case STATION_WRONG_PASSWORD:
            return WL_CONNECT_FAILED;
        case STATION_IDLE:
            return WL_IDLE_STATUS;
        default:
            return WL_DISCONNECTED;
    }
}

#ifndef LEGACY
void connect()
{
    char a[100];
    ip_info info;
    wifi_get_ip_info(STATION_IF, &info);
    wifi_station_disconnect();
    wifi_set_opmode(STATION_MODE);

    struct station_config stationConf;
    stationConf.bssid_set = 0;
    memcpy(&stationConf.ssid, ssid, 32);
    memcpy(&stationConf.password, password, 64);

    if (!wifi_station_set_config(&stationConf))
    {
        connection_state = ESPDISCONNECTED;
        return;
    }
    // WPA2 Enterprise
    wifi_station_set_wpa2_enterprise_auth(1);

    if (wifi_station_set_enterprise_username (ent_username, strlen((char*)ent_username)) ||
        wifi_station_set_enterprise_password (ent_password, strlen((char*)ent_password)))
    {
        connection_state = ESPDISCONNECTED;
        return;
    }

    if (!wifi_station_connect())
    {
        connection_state = ESPDISCONNECTED;
        return;
    }

    unsigned long int timeNow = millis();
    while ( wifi_status() != WL_CONNECTED )
    {
        ESP.wdtFeed();
        delay(1000);

        if ((unsigned long int) millis() - timeNow > 10000)
        {
            connection_state = ESPDISCONNECTED;
            wifi_station_disconnect();
            return;
        }
    }

    connection_state = ESPCONNECTED;
}
#else
void connect()
{
    WiFi.mode(WIFI_STA);
    const char* ssid     = "Nygma Tech Corp.";
    const char* password = "aristoteles de samos";
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    connection_state = ESPCONNECTED;
}
#endif

void hello_host(WiFiClient * client)
{
    unsigned long int timeNow = millis();

    while (!client->connect(host, port) && (unsigned long int) millis() - timeNow < 5000)
        delay(0);

    //check if connection was successfull
    if (!client->connected())
        client_state = CLIENTDISCONNECTED;
    else
        client_state = CLIENTCONNECTED;
}

void sync_clock()
{
    if ( timeClient->update() )
        clock_state = CLOCKSYNCED;
    else
        clock_state = CLOCKNOTSYNCED;
}

void load_files(WiFiClientSecure * client)
{
    #ifdef SECURE_CONNECTION
        client->loadCertificate(ca);
        client->loadPrivateKey(key);
    #endif
}
