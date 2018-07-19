
#include <uart.h>
#include <utility/string.h>
#include <alarm.h>
#include <machine/cortex/esp8266.h>

using namespace EPOS;

OStream cout;

int main(){

    Machine::delay(3*1000*1000);

    cout << "EMOTE + ESP Test\n" << endl;
    ESP8266 esp8266;
    char response[1500];

    char * command = "AT+SYSTEMREADY";
    char * expec = "OK";
    esp8266.send_command(command, strlen(command));
    esp8266.wait_response(expec, 50000);

    char * url = "httpbin.org/deny";
    esp8266.get(url, response, sizeof(response));

    char * post_url = "httpbin.org/post";
    char * payload = "{\"temperature\":23}";
    esp8266.post(post_url, payload, strlen(payload), response, sizeof(response));

    unsigned long now = esp8266.now();

    ESP8266::RSSI rssi = esp8266.rssi();
    
    while(1);

    return 0;
}
