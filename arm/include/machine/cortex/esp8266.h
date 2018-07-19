// EPOS ESP8266 NIC Mediator

#include <system/config.h>
#if !defined(__esp8266_h) && defined(__mmod_emote3__)

#define __esp8266_h

#include <machine.h>
#include <uart.h>
#include <utility/string.h>
#include <alarm.h>
#include <nic.h>
#include <utility/debug.h>

__BEGIN_SYS

/*
    Copy of the abstract implementation of a NIC, to implement a ESP8266 running custom firmware as a NIC.
    
    To use it, connect the ESP8266 - eMote3 pins as follows (assuming UNIT = 0, enabled by default):
    
    ESP8266 TX - A0 emote3
    ESP8266 RX - A1 eMote3

    Use the example in app/esp8266.cc and app/esp8266_traits.h to get you started.

    Known bugs and TODOS:
        - Some functions are not yet tested.
        - Retrieving large files with ESP8266::get can crash the ESP.
        - Some certificates like pastebin.com can crash the ESP.
        - Eduroam may eventually crash the ESP on startup.
        - For ESP SDK limitations, true secure connections can't be established. Possible solutions are:
            * Hardcoding the SHA1 fingerprint - Can change over time.
            * Establishing a root certificate - Just bad.
        
*/
class ESP8266 : NIC
{
public:

    typedef int RSSI;
    
    // Current implementation of ESP8266 firmware will only support HTTP and HTTPS. MQTT might be added later.
    enum {HTTP, HTTPS};

    ESP8266();
    ~ESP8266() {};
    
    RTC::Microsecond now();
 
    // TODO
    int send(const Address, const Protocol &prot, const void *data, unsigned int size) { return 0; }
    int receive(Address *src, Protocol *prot, void *data, unsigned int size) { return 0; }

    bool restart()
    { 
        // this is probably a bad practice, as it will timeout on connection fail
        // 10 seconds delay is needed because WPA2-Enterprise like eduroam can be slow on connecting
        db<ESP8266>(TRC) << "ESP8266::restart()" << endl;
        send_command("AT+RESTART", strlen("AT+RESTART"));
        return wait_response("READY", 10*1000*1000);
    }

    // TODO
    void on();
    void off();
    void reset()
    {
        off();
        Machine::delay(500000);
        on();
    }

    unsigned int mtu() { return 0; }
    const Address address();
    const Statistics statistics();
    RSSI rssi();

public:

    unsigned int send_command(const char *command, unsigned int size);
    unsigned int wait_response(const char * expected, const RTC::Microsecond & timeout, char * response = 0, unsigned int response_length = 0);
    unsigned int post(const char * url, const void * payload, const unsigned int payload_size, void * data = 0, unsigned int data_size = 0); // TODO
    unsigned int get(const char * url, void * data = 0, unsigned int data_size = 0);
    
private:
    UART * _uart;
    GPIO * _pwrkey;
    unsigned int _method;
    static const unsigned int UNITS = Traits<ESP8266>::UNITS;

};

__END_SYS

#endif
