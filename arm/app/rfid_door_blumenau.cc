
/*
 
    Code that runs on the EPOS to grant access based on RFID tags, syncronizing and logging data to a remote server.

    @author Brunno Vanelli

*/


#include <transducer.h>
#include <utility/ostream.h>
#include <machine/cortex/esp8266.h>
#include <rfid_reader.h>
#include <periodic_thread.h>
#include <persistent_storage.h>

using namespace EPOS;

OStream cout;

// constants
const long DOOR_OPEN_TIME = 15 * 1000000;
static const long SYNC_PERIOD = 60 * 1000000;

// declarations
int open_door(GPIO * door_control_pin);
class UIDRecord;
typedef UIDRecord Data;

/*

    hash runs a 32-bit hash function in an input string, based on stackoverflow.com/questions/2351087

    @param string that needs to be converted.
    @return unsigned int representation of string.
 
*/
unsigned int hash(char *str)
{
    unsigned int h;
    unsigned char *p;

    h = 0;
    for (p = (unsigned char*)str; *p != '\0'; p++)
        h = 37 * h + *p;

    db<Application>(TRC) << "hash(h=" << h << ")" << endl;
    return h;
}

/*

    atou converts a string into an unsigned int.

    @param string that needs to be converted.
    @return unsigned int representation of string.
 
*/
unsigned int atou(const char *s)
{
    unsigned int ret = 0;

    while ((*s >= '0') && (*s <= '9')) {
       ret = ret * 10 + *s++ - '0';
    }

    return ret;
}

// template the database based on the internal storage available
template<typename T, unsigned int N = Persistent_Storage::SIZE / (((sizeof(T) + sizeof(Persistent_Storage::Word) - 1) / sizeof(Persistent_Storage::Word)) * sizeof(Persistent_Storage::Word))>
/*

    Reimplementation of Persistent_Ring on EPOS source code to add more functionality.

    Original code available at include/machine/cortex/persistent_storage.h
 
*/
class Persistent_Ring
{
private:
    typedef Persistent_Storage::Address Address;

    static const Address START_ADDR = 0;
    static const Address END_ADDR = START_ADDR + sizeof(Address);
    static const Address DATA_ADDR = END_ADDR + sizeof(Address);
    static const Address EMPTY = 1 << (8 * sizeof(Address) - 1);

public:
    static const unsigned int SIZEOF_T = ((sizeof(T) + sizeof(Persistent_Storage::Word) - 1) / sizeof(Persistent_Storage::Word)) * sizeof(Persistent_Storage::Word);
    static const unsigned int SIZE = N * SIZEOF_T < Persistent_Storage::SIZE / SIZEOF_T * SIZEOF_T ?
        N * SIZEOF_T : Persistent_Storage::SIZE / SIZEOF_T * SIZEOF_T;

    static void clear() {
        Address start = DATA_ADDR;
        Address end = DATA_ADDR | EMPTY;

        Persistent_Storage::write(START_ADDR, &start, sizeof(Address));
        Persistent_Storage::write(END_ADDR, &end, sizeof(Address));
    }

    static void push(const T & t) {
        Address end = read_end();
        if(end & EMPTY)
            push_first(t, end);
        else {
            Address start = read_start();

            Address new_end;
            Address new_start = start;
            if(end + SIZEOF_T > SIZE)
                new_end = DATA_ADDR + SIZEOF_T;
            else
                new_end = end + SIZEOF_T;

            if(((end <= start) && (new_end > start)) ||
                    ((new_end < end) && ((new_end > start) || (end <= start)))) {
                if(start + SIZEOF_T > SIZE)
                    new_start = DATA_ADDR + SIZEOF_T;
                else
                    new_start += start + SIZEOF_T;
            }

            Persistent_Storage::write(new_end - SIZEOF_T, &t, SIZEOF_T);

            if(new_start != start)
                Persistent_Storage::write(START_ADDR, &new_start, sizeof(Address));

            Persistent_Storage::write(END_ADDR, &new_end, sizeof(Address));
        }
    }

    static bool pop(T * t) {
        Address end = read_end();
        if(end & EMPTY)
            return false;

        Address new_end = end;
        Address new_start;

        Address start = read_start();
        if(start + SIZEOF_T > SIZE)
            new_start = DATA_ADDR + SIZEOF_T;
        else
            new_start = start + SIZEOF_T;

        if((start < end) && (new_start >= end) ||
                ((new_start < start) && (end > start) || (new_start >= end))) {
            new_end = end | EMPTY;
        }

        Persistent_Storage::read(start, t, sizeof(T));
        if(new_end != end)
            Persistent_Storage::write(END_ADDR, &new_end, sizeof(Address));

        Persistent_Storage::write(START_ADDR, &new_start, sizeof(Address));

        return true;
    }

    static bool pop_filo(T * t) {
        Address end = read_end();
        if(end & EMPTY)
            return false;

        Address start = read_start();
        Address new_start = start;
        Address new_end = end - SIZEOF_T;

        if((end > start) && (new_end <= start)) {
            new_end = end | EMPTY;
        }

        Persistent_Storage::read(new_end, t, sizeof(T));
        Persistent_Storage::write(END_ADDR, &new_end, sizeof(Address));

        return true;
    }

    static bool read(T * t, unsigned int pos) {
        Address end = read_end();
        if(end & EMPTY || pos >= ring_size())
            return false;

        Address start = read_start() + sizeof(T) * pos;

        Persistent_Storage::read(start, t, sizeof(T));
        return true;
    }

    static unsigned int ring_size() {
        Address end = read_end();
        if(end & EMPTY)
            return 0;
        return (end - read_start()) / sizeof(T);
    }

private:
    static Address read_start() { return Persistent_Storage::read(START_ADDR); }
    static Address read_end() { return Persistent_Storage::read(END_ADDR); }

    static void push_first(const T & t, Address end) {
        end = end & ~EMPTY;
        if(end + SIZEOF_T > SIZE)
            end = DATA_ADDR + SIZEOF_T;
        else
            end += SIZEOF_T;

        Persistent_Storage::write(end - SIZEOF_T, &t, SIZEOF_T);
        Persistent_Storage::write(END_ADDR, &end, sizeof(Address));
    }
};

/*
 
    UIDRecord abstraction for internal database.
 
*/
class UIDRecord {
    
    public:
        
        UIDRecord (unsigned int uid = 0, unsigned int timestamp = 0, unsigned int status = 0) {
            this->uid = uid;
            this->timestamp = timestamp;
            this->status = status;
        }

        friend OStream & operator<<(OStream & os, const UIDRecord & u) {
            os << "{uid=" << u.uid << ",ts=" << u.timestamp << ",status=" << u.status << "}" << endl;
            return os;
        }

        enum {
            RECORD = 0,    // Local DB to check for UIDS
            UNCOMMITED_GRANTED = 1, // Record that need to be commited to the cloud server
            UNCOMMITED_DENIED = 2
        };

        static bool test(unsigned int uid) {
            for (unsigned int i = 0; i < Persistent_Ring_FIFO<Data>::ring_size(); i++) {
                Data k;
                Persistent_Ring_FIFO<Data>::read(&k, i);

                if (k.uid == uid && k.status == RECORD) {
                    return true;
                }
            }
            return false;
        }

        static bool get_next_uncommited(UIDRecord * u) {
            unsigned int size = Persistent_Ring_FIFO<Data>::ring_size();

            if (size == 0 || !Persistent_Ring_FIFO<Data>::read(u, size - 1))
                return false;

            return (u->status == UNCOMMITED_GRANTED || u->status == UNCOMMITED_DENIED);
        }

        static unsigned int dbhash() {
            char buffer[1500] = {0};
            for (unsigned int i = 0; i < Persistent_Ring_FIFO<Data>::ring_size(); i++) {
                Data k;
                Persistent_Ring_FIFO<Data>::read(&k, i);

                if (k.status == RECORD) {
                    char str_uid[11] = {0};
                    utoa(k.uid, str_uid);
                    strcat(buffer, str_uid);
                    strcat(buffer, "\r\n");
                }
            }
            strcat(buffer, "\r"); // from ESP
            db<Application>(INF) << "UIDRecord::dbhash(b=" << buffer << ")" << endl;
            return hash(buffer);
        }

        unsigned int uid;
        unsigned int timestamp;
        unsigned int status;
};

/*

    syncronize will syncronize the device with the remote database.
   
    @param Object for NIC ESP8266 mediator.
    @param String for the remote server url or IP address. 
    @return int to ensure it can be called as a thread.

*/
int synchronize (ESP8266 * nic, char * url)
{
    unsigned int last_hash = UIDRecord::dbhash(); // test hash for every response. don't update the cache if hash is the same, saving cache lifetime
    char response[1500]; // Response buffer
    
    while (true) {
        
        // First, commit all the stored requests to the database
        Data k;
        while (UIDRecord::get_next_uncommited(&k)) {
            char payload[100];
            char buffer[11];
            strcpy(payload, "uid=");
            utoa(k.uid, buffer);
            strcat(payload, buffer);
            strcat(payload, "&timestamp=");
            utoa(k.timestamp, buffer);
            strcat(payload, buffer);
            strcat(payload, "&code=");
            utoa(k.status, buffer);
            strcat(payload, buffer);

            unsigned int response_size = nic->post(url, payload, strlen(payload), response, sizeof(response));

            if (!response_size && strcmp(response, "=HTTP409\r") != 0) // HTTP409 means that the request is already on database
                Periodic_Thread::wait_next(); // if request fails, try next cycle
            else {
                Persistent_Ring_FIFO<Data>::pop_filo(&k);
                db<Application>(TRC) << "synchronize(uid=" << k.uid << ",f=Data posted)" << endl;
                Machine::delay(500000); // wait 500 ms between each request for no particular reason
            }
        }

        // Second, update local database with online latest version
        unsigned int response_size = nic->get(url, response, sizeof(response));
        if (response_size && hash(response) != last_hash)
        { 
            last_hash = hash(response);
            Persistent_Ring_FIFO<Data>::clear();                    
            char buffer[11] = {0}; 
            for (unsigned int i = 0, count = 0; i < response_size - 1; i++) {
                if (response[i] == '\r') {
                    buffer[count] = '\0';
                    // update cache
                    unsigned long uid = atou(buffer);
                    Data d(uid, 0, Data::RECORD);
                    Persistent_Ring_FIFO<Data>::push(d);                    
                    db<Application>(TRC) << "synchronize(uid=" << d.uid << ",f=Cache updated)" << endl;
                }
                else if (response[i] == '\n') {
                    count = 0; // starting new string
                }
                else {
                    buffer[count] = response[i];
                    count++;
                }
            }
        }
        Periodic_Thread::wait_next();
    }
    return 0;
}

/*

    rfid_reader runs in a thread and take as parameters an RFID_Reader object and a gpio and tests the device database for matching UIDs.

    @param Object of RFID reader.
    @param Object of GPIO actuator.
    @return int to ensure it can be called as a thread.

*/
int rfid_reader(RFID_Reader * reader, GPIO * door_control_pin)
{
    while (true) {
        RFID_Reader::UID u = reader->get();
        unsigned int uid = (unsigned int) u;

        if (UIDRecord::test(uid)) {
            db<Application>(INF) << "rfid_reader(uid=" << uid << ",status=Opening!)" << endl;
            Data d(uid, 0, UIDRecord::UNCOMMITED_GRANTED); // TODO: implement timestamp
            Persistent_Ring_FIFO<Data>::push(d);
            open_door(door_control_pin);
        }
        else {
            db<Application>(INF) << "rfid_reader(uid=" << uid << ",status=Denied!)" << endl;
            Data d(uid, 0, UIDRecord::UNCOMMITED_DENIED); // TODO: implement timestamp
            Persistent_Ring_FIFO<Data>::push(d);
        }

        reader->halt(u);
    }
    return 0;
}

int open_door(GPIO * door_control_pin) {
    door_control_pin->set();
    Machine::delay(DOOR_OPEN_TIME);
    door_control_pin->clear();
}

int main()
{
    Machine::delay(5000000);
    cout << "Smart Door" << endl;
    cout << "Here" << endl;

    // Declare RFID_Reader interface
    SPI spi(0, 4000000, SPI::FORMAT_MOTO_0, SPI::MASTER, 500000, 8);
    GPIO select('B', 5, GPIO::OUT);
    GPIO reset('C', 6, GPIO::INOUT);
    RFID_Reader reader(&spi, &select, &reset);
    GPIO door_control('B', 0, GPIO::OUT);
    // Dispatch thread
    Thread _door_control_thread(&rfid_reader, &reader, &door_control);
   
    // Same as original smart_door 
    //Switch_Sensor door_control_actuator(2, 'D', 1, GPIO::OUT);
    //Switch door_control(2, 100000, Switch::COMMANDED);

    ESP8266 nic;
    //char url[] = "192.168.1.62/api.php?id=abcdef&token=aaaaaaaaaaaaaa"; // This is being declared here because I can't pass this as parameter without getting a compiler error - Thread requires a static method on a class, however everything else must be static.
    char url[] = "192.168.1.62:8080/api/v1/abcdef/aaaaaaaaaaaaaa/";
    Periodic_Thread _esp_synchronizer_thread(SYNC_PERIOD, &synchronize, &nic, url);
    
    // suspend main thread
    Thread::self()->suspend();

    return 0;
}
