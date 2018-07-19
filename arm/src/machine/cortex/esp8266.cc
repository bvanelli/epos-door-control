#include <system/config.h>
#if defined(__mmod_emote3__)

#include <machine/cortex/machine.h>
#include <machine/cortex/esp8266.h>
#include <machine/cortex/uart.h>

__BEGIN_SYS

ESP8266::ESP8266()
{
    _uart = new (SYSTEM) UART(Traits<ESP8266>::UART_UNIT,
                              Traits<ESP8266>::UART_BAUD_RATE,
                              Traits<ESP8266>::UART_DATA_BITS,
                              Traits<ESP8266>::UART_PARITY,
                              Traits<ESP8266>::UART_STOP_BITS);
    
    _method = Traits<ESP8266>::METHOD;
    _pwrkey = new (SYSTEM) GPIO(Traits<ESP8266>::PWRKEY_PORT, Traits<ESP8266>::PWRKEY_PIN, GPIO::OUT);
}

unsigned int ESP8266::send_command(const char *command, unsigned int size)
{
    db<ESP8266>(TRC) << "ESP8266::send_command(c=" << command << ",size=" << size << ")" << endl;

    if(_uart->ready_to_get())
        while(_uart->ready_to_get())
            db<ESP8266>(INF) << _uart->get();

    for(unsigned int i = 0; i < size; i++)
        _uart->put(command[i]);
    _uart->put('\r');

    return size;
}

unsigned int ESP8266::wait_response(const char * expected, const RTC::Microsecond & timeout, char * response, unsigned int response_size)
{
    db<ESP8266>(TRC) << "ESP8266::wait_response(ex=" << expected << ",tmt=" << timeout << ")" << endl;

    TSC::Time_Stamp end = TSC::time_stamp() + timeout * TSC::frequency() / 1000000;

    const char error[] = "ERROR";
    unsigned int ret = 1;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int len = strlen(expected);
    while(i < len) {
        while(!(_uart->ready_to_get()) && ((timeout == 0) || (TSC::time_stamp() < end)));

        if(!_uart->ready_to_get()) {
            db<ESP8266>(WRN) << "ESP8266::wait_response(ex=" << expected << ",tmt=" << timeout << ") => Timeout!" << endl;
            ret = 0;
            break;
        }

        char c = _uart->get();
        db<ESP8266>(INF) << c;
        if(c == expected[i])
            i++;
        else if(c == expected[0])
            i = 1;
        else
            i = 0;

        if(c == error[j]) {
            j++;
            if(j >= strlen(error)) {
                db<ESP8266>(WRN) << "ESP8266::wait_response(ex=" << expected << ",tmt=" << timeout << ") => Error!" << endl;
                ret = 0;
                break;
            }
        }
        else if(c == error[0])
            j = 1;
        else
            j = 0;
    }

    if(response) {
        for(unsigned int i = 0; i < response_size; i++) {
            while(!(_uart->ready_to_get()) && ((timeout == 0) || (TSC::time_stamp() < end)));

            if(!_uart->ready_to_get()) {
                //db<UART>(WRN) << "ESP8266::wait_response(ex=" << expected << ",tmt=" << timeout << ") => Timeout!" << endl;
                //ret = false; // because there is no termination, break as soon as the timeout or response_size is reached
                // Also, because there is no termination all wait_responses will always timeout (assuming proper response buffer), because there is no way to tell if the stream ended or just hanged. Make sure your application can wait the timeout time, reduce it or push it to a thread
                response[i] = '\0';
                ret = ret * i;
                break;
            }
            response[i] = _uart->get();
            db<ESP8266>(INF) << response[i];
            /* Not terminating can be bad, but the HTTP response most likely contain \n\r sequences
            if(response[i] == '\n')
                break;
            */
        }
    }

    while(_uart->ready_to_get())
        db<ESP8266>(WRN) << _uart->get(); // Warning: not enough response buffer size

    db<ESP8266>(INF) << "ESP8266::wait_response(ret=" << ret << ")" << endl;
    return ret;
}

unsigned int ESP8266::get(const char * url, void * data, unsigned int data_size)
{
    db<ESP8266>(TRC) << "ESP8266::get(url=" << url << ")" << endl;
    char command[9 + strlen(url)];
    char * response = reinterpret_cast<char *>(data);
   
    if (_method == HTTP)
        strcpy(command, "AT+GET=");
    else if (_method == HTTPS)
        strcpy(command, "AT+GETS=");
    else
        return 0;
    strcat(command, url);

    send_command(command, strlen(command));
    return wait_response("OK=", 5*1000*1000, response, data_size); // Hardcoding the timeout can be dangerous
}

unsigned int ESP8266::post(const char * url, const void * payload, const unsigned int payload_size, void * data, unsigned int data_size)
{
    db<ESP8266>(TRC) << "ESP8266::post(url=" << url << ")" << endl;

    char payload_command[12 + payload_size];
    strcpy(payload_command, "AT+PAYLOAD="); 
    for(unsigned int i = 0; i < payload_size; i++) {
        payload_command[11 + i] = reinterpret_cast<const char*>(payload)[i];
    }
    payload_command[12 + payload_size - 1] = '\0';
    send_command(payload_command, strlen(payload_command));

    if (!wait_response("OK", 30*1000))
        return 0;

    char command[10 + strlen(url)];
    char * response = reinterpret_cast<char *>(data);
    
    if (_method == HTTP)
        strcpy(command, "AT+POST=");
    else if (_method == HTTPS)
        strcpy(command, "AT+POSTS=");
    else
        return 0;
    strcat(command, url);

    send_command(command, strlen(command));
    return wait_response("OK=", 5*1000*1000, response, data_size); // Hardcoding the timeout can be dangerous
}

RTC::Microsecond ESP8266::now()
{
    db<ESP8266>(TRC) << "ESP8266::now()" << endl;
    char buf[10];
    RTC::Microsecond time_stamp;
    send_command("AT+TIMESTAMP", strlen("AT+TIMESTAMP"));
    if (wait_response("OK=", 500000, buf, 10)) {
       time_stamp = atol(buf);
       db<ESP8266>(INF) << "ESP8266::now(ts=" << time_stamp << ")" << endl;
       return time_stamp;
    }
    else {
        return 0;
    }
}

ESP8266::RSSI ESP8266::rssi()
{
    db<ESP8266>(TRC) << "ESP8266::rssi()" << endl;
    char buf[5];
    ESP8266::RSSI rssi;
    send_command("AT+RSSI", strlen("AT+RSSI"));
    if (wait_response("OK=", 5000, buf, 5)) {
       rssi = atol(buf);
       db<ESP8266>(INF) << "ESP8266::rssi(rssi=" << rssi << ")" << endl;
       return rssi;
    }
    else {
        return 0; // FIXME: what if rssi=0?
    }
}

/* FIXME: Compiler says 'Address' does not have a name type
const Address ESP8266::address()
{
    db<ESP8266>(TRC) << "ESP8266::address()" << endl;
    char buf[10];
    send_command("AT+GETIP", strlen("AT+GETIP"));
    if (wait_response("OK=", 5000, buf, 4)) {
       unsigned int ip = atol(buf);
       db<ESP8266>(INF) << "ESP8266::address(ip=" << ip << ")" << endl;
       return NIC_Common::Address(ip);
    }
    else {
        return 0;
    }
}
*/

__END_SYS

#endif
