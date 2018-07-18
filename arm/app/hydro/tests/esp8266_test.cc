#include <gpio.h>
#include <machine.h>
#include <utility/ostream.h>
#include <machine/cortex/esp8266.h>

using namespace EPOS;

const char DOMAIN[]   = "hydro";
const char USERNAME[] = "hydro";
const char PASSWORD[] = "3ab0fd";

const char ssid[] = "LISHA";
const char pass[] = "@s@ferl1sh@";

ESP8266 * esp;
OStream cout;

int main()
{
    cout << "main()" << endl;

    // Initialize mediators
    /* TODO:
     * Transfer the uart, gpio, host and routes info to the traits 
     * Create a esp8266_init to initiate the mediator (use the same pattern as the m95 mediator)
    */
    UART uart(0, 115200, 8, 0, 1);
    GPIO rst('C', 0, GPIO::OUT);
    cout << "ESP INIT" << endl;
    esp = new ESP8266(&uart, &rst);
    cout << "ESP CONNECT WITH WiFi" << endl;
    esp->connect(ssid, sizeof(ssid)-1, pass, sizeof(pass)-1);
    cout << "CONNECTED!" << endl;

    while(true)
    {
        Machine::delay(5000000);
        cout << "Time: " << esp->now() << endl;
    }

    return 0;
}
