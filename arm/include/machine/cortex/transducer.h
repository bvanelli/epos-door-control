// EPOS ARM Cortex Smart Transducer Declarations

#ifndef __cortex_transducer_h
#define __cortex_transducer_h

#include <smart_data.h>

#include <keyboard.h>
#include <smart_plug.h>
#include <hydro_board.h>
#include <gpio.h>
#include <tstp.h>
#include <tsc.h>
#include <rfid_reader.h>
#include <persistent_storage.h>
#include <condition.h>
#include <spi.h>
#include <rs485.h>
#include "hcsr04.h"
#include <machine.h>

__BEGIN_SYS

class Keyboard_Sensor: public Keyboard
{
public:
    static const unsigned int UNIT = TSTP::Unit::Acceleration;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

    typedef Keyboard::Observer Observer;
    typedef Keyboard::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Keyboard_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Keyboard_Sensor> * data) {
        if(ready_to_get())
            data->_value = get();
        else
            data->_value = -1;
    }

    static void actuate(unsigned int dev, Smart_Data<Keyboard_Sensor> * data, const Smart_Data<Keyboard_Sensor>::Value & command) {}
};

#ifdef __mmod_emote3__

template<unsigned int _U, unsigned int _N=TSTP::Unit::I32, int _E=0, bool _I=false, bool _P=true>
class Dummy_Transducer
{
public:
    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

    static const unsigned int UNIT = _U;
    static const unsigned int NUM = _N;
    static const int ERROR = _E;
    static const bool INTERRUPT = _I;
    static const bool POLLING = _P;
public:
    template<typename S>
    static void sense(unsigned int dev, S * data) { }
    template<typename S>
    static void actuate(unsigned int dev, S * data, const typename S::Value & command) {
        data->_value = command;
    }
    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }
private:
    static Observed _observed;
};

/*
 * Capacitive Transmitters Series 46 X / 46 X Ei
 * 
 * Reference: http://www.keller-druck.com/home_e/paprod_e/46x_e.asp
*/
class Pressure_Sensor_Keller_Capacitive: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Pressure_Sensor_Keller_Capacitive(unsigned int dev, unsigned int hydro_relay_power, unsigned int hydro_adc) {
        _hydro_relay_power = hydro_relay_power;
        _hydro_adc = hydro_adc;
    }

    static void sense(unsigned int dev, Smart_Data<Pressure_Sensor_Keller_Capacitive> * data) {
        Hydro_Board::on(_hydro_relay_power);
        Machine::delay(2000000);
        data->_value = Hydro_Board::read(_hydro_adc);
        Hydro_Board::off(_hydro_relay_power);
    }

    static void actuate(unsigned int dev, Smart_Data<Pressure_Sensor_Keller_Capacitive> * data, const Smart_Data<Pressure_Sensor_Keller_Capacitive>::Value & command) {}

private:
    static unsigned int _hydro_relay_power;
    static unsigned int _hydro_adc;
};

class Water_Level_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Water_Level_Sensor> * data) {
        data->_value = level(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Level_Sensor> * data, const Smart_Data<Water_Level_Sensor>::Value & command) {}
};

class Water_Level_Sensor_Ultrasonic_Microflex: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
     Water_Level_Sensor_Ultrasonic_Microflex(unsigned int dev, unsigned int hydro_relay_power, unsigned int hydro_adc) {
        _hydro_relay_power = hydro_relay_power;
        _hydro_adc = hydro_adc;
    }

    static void sense(unsigned int dev, Smart_Data<Water_Level_Sensor_Ultrasonic_Microflex> * data) {
        Hydro_Board::on(_hydro_relay_power);
        Machine::delay(20000000);
        data->_value = Hydro_Board::read(_hydro_adc);
        Hydro_Board::off(_hydro_relay_power);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Level_Sensor_Ultrasonic_Microflex> * data, const Smart_Data<Water_Level_Sensor_Ultrasonic_Microflex>::Value & command) {}

private:
    static unsigned int _hydro_relay_power;
    static unsigned int _hydro_adc;
};

class Water_Turbidity_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Amount_of_Substance; // TODO
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Water_Turbidity_Sensor> * data) {
        data->_value = turbidity(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Turbidity_Sensor> * data, const Smart_Data<Water_Turbidity_Sensor>::Value & command) {}
};

class Water_Turbidity_Sensor_Solar: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Amount_of_Substance; // TODO
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Water_Turbidity_Sensor_Solar(unsigned int dev, unsigned int hydro_relay_power, unsigned int hydro_adc, unsigned int hydro_relay_infrared, unsigned int hydro_adc_infrared){
        _hydro_relay_power = hydro_relay_power;
        _hydro_adc = hydro_adc;
    }

    static void sense(unsigned int dev, Smart_Data<Water_Turbidity_Sensor_Solar> * data) {
        Hydro_Board::on(_hydro_relay_power);
        // Filter daylight as directed by sensor manufacturer
        Machine::delay(3250000);
        int daylight = Hydro_Board::read(_hydro_adc);
        Hydro_Board::on(_hydro_relay_infrared);
        Machine::delay(450000);
        int mixed = Hydro_Board::read(_hydro_adc_infrared);
        Hydro_Board::off(_hydro_relay_infrared);
        Hydro_Board::off(_hydro_relay_power);
        if(mixed - daylight < 0)
        {
            data->_value = 0;
        }
        else
        {
            data->_value = mixed - daylight;
        }
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Turbidity_Sensor_Solar> * data, const Smart_Data<Water_Turbidity_Sensor_Solar>::Value & command) {}

private:
    static unsigned int _hydro_relay_power;
    static unsigned int _hydro_adc;
    static unsigned int _hydro_relay_infrared;
    static unsigned int _hydro_adc_infrared;
};

/*
 *Campbell Scientific OBS-3+
 *
 * Ranges: (depends on sediment size, particle shape, and reflectivity)
 *  Turbidity (low/high): 250/1000 NTU; 500/2000 NTU; 1000/4000 NTU
 *  Mud: 5000 to 10,000 mg L–1
 *  Sand: 50,000 to 100,000 mg L–1
 * Accuracy: (whichever is larger)
 *  Turbidity: 2% of reading or 0.5 NTU
 *  Mud: 2% of reading or 1 mg L–1
 *  Sand: 4% of reading or 10 mg L–1
 * Resolution:
 *  16-bit ADC - This is what is supported!
 *      Turbidity: 0.004/0.01 NTU; 0.008/0.03 NTU; 0.01/0.06 NTU
 *  12-bit ADC
 *      Turbidity: 0.06/0.2 NTU; 0.1/0.5 NTU; 0.2/1.0 NTU
 *
 * Minimum stabilization time: 2s
 * Maximum data rate = 10Hz (100ms/sample)
*/
class Water_Turbidity_Sensor_OBS: public Hydro_Board
{
public:
    static const unsigned int MAX_DEVICES = 4;
    static const unsigned int UNIT = TSTP::Unit::Amount_of_Substance; // TODO
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Water_Turbidity_Sensor_OBS(unsigned int dev, unsigned int hydro_relay_power, unsigned int hydro_adc){
        assert(dev < MAX_DEVICES);
        _dev[dev] = this;
        _hydro_relay_power = hydro_relay_power;
        _hydro_adc = hydro_adc;
    }

    static void sense(unsigned int dev, Smart_Data<Water_Turbidity_Sensor_OBS> * data) {
        Machine::delay(250000); // Wait 250 ms before reading
        Hydro_Board::on(_hydro_relay_power);
        Machine::delay(2000000);
        data->_value = Hydro_Board::read(_hydro_adc);
        Hydro_Board::off(_hydro_relay_power);
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Turbidity_Sensor_OBS> * data, const Smart_Data<Water_Turbidity_Sensor_OBS>::Value & command) {}

private:
    static Water_Turbidity_Sensor_OBS * _dev[MAX_DEVICES];
    static unsigned int _hydro_relay_power;
    static unsigned int _hydro_adc;
};

class Water_Flow_Sensor: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Water_Flow;// m^3/s

    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Water_Flow_Sensor> * data) {
        
        //Convert pulses to liters/s

        data->_value = water_flow();
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Flow_Sensor> * data, const Smart_Data<Water_Flow_Sensor>::Value & command) {}
};

//For now, leave as a class apart from the generic Water_Flow_Sensor
class Water_Flow_Sensor_WSTAR: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Water_Flow;// m^3/s
    
    static const unsigned int NUM = TSTP::Unit::I32;
    //static const char FACTOR = TSTP::Unit::Factor::MILI;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    static const unsigned int LITRES_PER_PULSE = 100;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Water_Flow_Sensor_WSTAR> * data) {
        int pulses = water_flow();
        data->_value = pulses * LITRES_PER_PULSE;
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Flow_Sensor_WSTAR> * data, const Smart_Data<Water_Flow_Sensor_WSTAR>::Value & command) {}
};

//For now, leave as a class apart from the generic Water_Flow_Sensor
class Water_Flow_Sensor_M170: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Water_Flow;// m^3/s

    static const unsigned int NUM = TSTP::Unit::I32;
    //static const char FACTOR = TSTP::Unit::Factor::MILI;    
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    static const unsigned int LITRES_PER_PULSE = 10;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Water_Flow_Sensor_M170> * data) {
        int pulses = water_flow();
        data->_value = pulses * LITRES_PER_PULSE;
    }

    static void actuate(unsigned int dev, Smart_Data<Water_Flow_Sensor_M170> * data, const Smart_Data<Water_Flow_Sensor_M170>::Value & command) {}
};

class Pluviometer: public Hydro_Board
{
public:
    static const unsigned int UNIT = TSTP::Unit::Length; // mm
    //static const char FACTOR = TSTP::Unit::Factor::MILI;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Pluviometer> * data) {
        data->_value = rain();
    }

    static void actuate(unsigned int dev, Smart_Data<Pluviometer> * data, const Smart_Data<Pluviometer>::Value & command) {}
};

class Active_Power_Sensor: public Smart_Plug
{
public:
    static const unsigned int UNIT = TSTP::Unit::Power;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef Smart_Plug::Observer Observer;
    typedef Smart_Plug::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    static void sense(unsigned int dev, Smart_Data<Active_Power_Sensor> * data) {
        data->_value = power(dev);
    }

    static void actuate(unsigned int dev, Smart_Data<Active_Power_Sensor> * data, const Smart_Data<Active_Power_Sensor>::Value & command) {
        Smart_Plug::actuate(dev, command);
    }
};

class ADC_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::Luminous_Intensity;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    ADC_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<ADC_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.read();
    }

    static void actuate(unsigned int dev, Smart_Data<ADC_Sensor> * data, const Smart_Data<ADC_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class Temperature_Sensor // TODO
{
public:
    static const unsigned int UNIT = TSTP::Unit::Temperature;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Temperature_Sensor() {
    }

    static void sense(unsigned int dev, Smart_Data<Temperature_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        int val = adc.read();
        float tc;
        if (val > 0) {
            float delta;
            delta = (10000.0*(2047-val))/val;
            float ldelta = logf(delta, E);
            tc = (1/(0.0011253088 + (0.0002347118)*ldelta + 0.00000008 * (ldelta*ldelta*ldelta))) - 273.0;
        } else {
            tc = -273.0;
        }
        // Integer value of Temperature
        data->_value = (int)(tc + 0.5);
    }

    static void actuate(unsigned int dev, Smart_Data<Temperature_Sensor> * data, const Smart_Data<Temperature_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class Switch_Sensor: public GPIO
{
    static const unsigned int MAX_DEVICES = 8;

public:
    static const unsigned int UNIT = TSTP::Unit::Switch;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Switch_Sensor(unsigned int dev, char port, unsigned int pin, const GPIO::Direction & dir, const GPIO::Pull & p = GPIO::UP)
    : GPIO(port, pin, dir, p) {
        assert(dev < MAX_DEVICES);
        _dev[dev] = this;
    }

    Switch_Sensor(unsigned int dev, char port, unsigned int pin, const GPIO::Direction & dir, const GPIO::Pull & p, const GPIO::Edge & int_edge)
    : GPIO(port, pin, dir, p, &int_handler, int_edge) {
        assert(dev < MAX_DEVICES);
        _dev[dev] = this;
    }

    static void sense(unsigned int dev, Smart_Data<Switch_Sensor> * data) {
        assert(dev < MAX_DEVICES);
        data->_value = _dev[dev]->get();
    }

    static void actuate(unsigned int dev, Smart_Data<Switch_Sensor> * data, const Smart_Data<Switch_Sensor>::Value & command) {
        assert(dev < MAX_DEVICES);
        _dev[dev]->set(command);
        data->_value = command;
    }

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static void int_handler(const IC::Interrupt_Id & id) { notify(); }

    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
    static Switch_Sensor * _dev[MAX_DEVICES];
};

class RFID_Sensor
{
    static const unsigned int MAX_DEVICES = 8;

public:
    static const unsigned int UNIT = TSTP::Unit::RFID;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = true;
    static const bool POLLING = false;

    typedef RFID_Reader::Observer Observer;
    typedef RFID_Reader::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    class Data : private RFID_Reader::UID
    {
    public:
        enum Code {
            DENIED     = 0,
            OPEN_NOW   = 1 << 0,
            AUTHORIZED = 1 << 1,
        };

        Data() : _code(0) {}
        Data(unsigned int v) : RFID_Reader::UID(v >> 8), _code(v) {}
        Data(const RFID_Reader::UID & u) : RFID_Reader::UID(u) {}
        Data(const RFID_Reader::UID & u, unsigned char code) : RFID_Reader::UID(u), _code(code) {}

        operator unsigned int() const {
            unsigned int i = RFID_Reader::UID::operator unsigned int();
            return (i << 8) | _code;
        }

        const RFID_Reader::UID & uid() const { return *this; }

        unsigned char code() const { return _code; }
        void code(unsigned char c) { _code = c; }

        bool authorized() const { return _code & AUTHORIZED; }
        void authorized(bool a) { if(a) _code |= AUTHORIZED; else _code &= ~AUTHORIZED; }
        bool open() const { return _code & OPEN_NOW; }
        void open(bool a) { if(a) _code |= OPEN_NOW; else _code &= ~OPEN_NOW; }

    private:
        unsigned char _code;
    }__attribute__((packed));

    RFID_Sensor(unsigned int dev, SPI * reader_spi, GPIO * reader_gpio0, GPIO * reader_gpio1)
    : _device(reader_spi, reader_gpio0, reader_gpio1)
    {
        assert(dev < MAX_DEVICES);
        _dev[dev] = this;
    }

    ~RFID_Sensor() {
        for(unsigned int i = 0; i < MAX_DEVICES; i++)
            if(_dev[i] == this)
                _dev[i] = 0;
    }

    static void sense(unsigned int dev, Smart_Data<RFID_Sensor> * data) {
        assert(dev < MAX_DEVICES);
        if(_dev[dev])
            _dev[dev]->sense(data);
    }

    static void actuate(unsigned int dev, Smart_Data<RFID_Sensor> * data, const Smart_Data<RFID_Sensor>::Value & command) {
        data->_value = command;
    }

    static void attach(Observer * obs) { RFID_Reader::attach(obs); }
    static void detach(Observer * obs) { RFID_Reader::detach(obs); }

private:
    void sense(Smart_Data<RFID_Sensor> * data) {
        if(_device.ready_to_get()) {
            Data id_with_code = _device.get();
            data->_value = id_with_code;
        } else
            data->_value = 0;
    }

private:
    RFID_Reader _device;
    static Observed _observed;
    static RFID_Sensor * _dev[MAX_DEVICES];
};

class Distance_Sensor : private HCSR04
{
    static const unsigned int MAX_DEVICES = 8;

public:
    static const unsigned int UNIT = TSTP::Unit::Length;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Distance_Sensor(unsigned int dev, GPIO * trigger, GPIO * echo) : HCSR04(trigger, echo) {
        assert(dev < MAX_DEVICES);
        _dev[dev] = this;
    }

    static void sense(unsigned int dev, Smart_Data<Distance_Sensor> * data) {
        assert(dev < MAX_DEVICES);
        if(_dev[dev]) {
            while(!_dev[dev]->ready_to_get());
            data->_value = _dev[dev]->get();
        }
    }

    static void actuate(unsigned int dev, Smart_Data<Distance_Sensor> * data, const Smart_Data<Distance_Sensor>::Value & command) {}

    static void attach(Observer * obs) {}
    static void detach(Observer * obs) {}

private:
    static Distance_Sensor * _dev[MAX_DEVICES];
};

class Vibration_Sensor
{
public:
    static const unsigned int UNIT = TSTP::Unit::Force;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Vibration_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Vibration_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.convert(adc.read(),3300);            
    }

    static void actuate(unsigned int dev, Smart_Data<Vibration_Sensor> * data, const Smart_Data<Vibration_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class Sound_Sensor
{
public:
    static const unsigned int UNIT = TSTP::Unit::Sound_Intensity;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

public:
    Sound_Sensor() {}

    static void sense(unsigned int dev, Smart_Data<Sound_Sensor> * data) {
        ADC adc(static_cast<ADC::Channel>(dev));
        data->_value = adc.convert(adc.read(),3300);
    }

    static void actuate(unsigned int dev, Smart_Data<Sound_Sensor> * data, const Smart_Data<Sound_Sensor>::Value & command) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static bool notify() { return _observed.notify(); }

    static void init();

private:
    static Observed _observed;
};

class SSB_Substation_Sensor
{
public:
    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

    static const unsigned int UNIT = Traits<UART>::DEF_UNIT;
    static const unsigned int BAUD_RATE = Traits<UART>::DEF_BAUD_RATE;
    static const unsigned int DATA_BITS = Traits<UART>::DEF_DATA_BITS;
    static const unsigned int PARITY = Traits<UART>::DEF_PARITY;
    static const unsigned int STOP_BITS = Traits<UART>::DEF_STOP_BITS;

    enum Quantity {
        CURRENT = 112,
        POWER_FACTOR = 176,
        ACTIVE_POWER = 241
    };

public:
    SSB_Substation_Sensor(unsigned int unit = UNIT, unsigned int baud_rate = BAUD_RATE, unsigned int data_bits = DATA_BITS, unsigned int parity = PARITY, unsigned int stop_bits = STOP_BITS) {
        if(_dev)
            delete _dev;
        _dev = new RS485(unit, baud_rate, data_bits, parity, stop_bits);
        _instances++;
    }

    ~SSB_Substation_Sensor() {
        if(--_instances == 0) {
            delete _dev;
            _dev = 0;
        }
    }

protected:
    static float sense(const char * command) {
        union {
            char c[4]; 
            float f;
        } conv;

        char res[16];
        _dev->write(command, 8);
        Machine::delay(800000);
        _dev->read(res, 16);
        conv.c[0] = 0;
        conv.c[1] = 0;
        conv.c[2] = res[5];
        conv.c[3] = res[4];
    
        int quantity_code = command[6];
        if(quantity_code == CURRENT)
            conv.f *= 40; // Current conversion factor 
        else if(quantity_code == ACTIVE_POWER)
            conv.f *= -40; // Active Power conversion factor 
        else if(quantity_code == POWER_FACTOR) {
            // The Power factor is always positive
            if(conv.f < 0)
                conv.f *= -1000; // Power factor value correction
            else
                conv.f *= 1000; // Power factor value correction
        }

        return conv.f;
    }

private:
    static unsigned int _instances;

    static RS485 * _dev;
};

template<unsigned int U>
class SSB_Substation_Common: protected SSB_Substation_Sensor
{
public:
    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

    static const unsigned int UNIT = U;
    static const unsigned int NUM = TSTP::Unit::F32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    static void attach(Observer * obs) {}
    static void detach(Observer * obs) {}
};


class SSB_Substation_Current: public SSB_Substation_Common<TSTP::Unit::Current>
{
public:
    static void sense(unsigned int dev, Smart_Data<SSB_Substation_Current> * data) { data->_value = SSB_Substation_Sensor::sense("\x01\x04\x00\x04\x00\x01\x70\x0B"); }
    static void actuate(unsigned int dev, Smart_Data<SSB_Substation_Current> * data, const Smart_Data<SSB_Substation_Current>::Value & command) {}
};

class SSB_Substation_Voltage_R: public SSB_Substation_Common<TSTP::Unit::Voltage>
{
public:
    static void sense(unsigned int dev, Smart_Data<SSB_Substation_Voltage_R> * data) { data->_value = SSB_Substation_Sensor::sense("\x01\x04\x00\x01\x00\x01\x60\x0A"); }
    static void actuate(unsigned int dev, Smart_Data<SSB_Substation_Voltage_R> * data, const Smart_Data<SSB_Substation_Voltage_R>::Value & command) {}
};

class SSB_Substation_Voltage_S: public SSB_Substation_Common<TSTP::Unit::Voltage>
{
public:
    static void sense(unsigned int dev, Smart_Data<SSB_Substation_Voltage_S> * data) { data->_value = SSB_Substation_Sensor::sense("\x01\x04\x00\x02\x00\x01\x90\x0A"); }
    static void actuate(unsigned int dev, Smart_Data<SSB_Substation_Voltage_S> * data, const Smart_Data<SSB_Substation_Voltage_S>::Value & command) {}
};

class SSB_Substation_Voltage_T: public SSB_Substation_Common<TSTP::Unit::Voltage>
{
public:
    static void sense(unsigned int dev, Smart_Data<SSB_Substation_Voltage_T> * data) { data->_value = SSB_Substation_Sensor::sense("\x01\x04\x00\x03\x00\x01\xC1\xCA"); }
    static void actuate(unsigned int dev, Smart_Data<SSB_Substation_Voltage_T> * data, const Smart_Data<SSB_Substation_Voltage_T>::Value & command) {}
};

class SSB_Substation_Active_Power: public SSB_Substation_Common<TSTP::Unit::Power>
{
public:
    static void sense(unsigned int dev, Smart_Data<SSB_Substation_Active_Power> * data) { data->_value = SSB_Substation_Sensor::sense("\x01\x04\x00\x0C\x00\x01\xF1\xC9"); }
    static void actuate(unsigned int dev, Smart_Data<SSB_Substation_Active_Power> * data, const Smart_Data<SSB_Substation_Active_Power>::Value & command) {}
};

class SSB_Substation_Average_Power_Factor: public SSB_Substation_Common<TSTP::Unit::Power>
{
public:
    static void sense(unsigned int dev, Smart_Data<SSB_Substation_Average_Power_Factor> * data) { data->_value = SSB_Substation_Sensor::sense("\x01\x04\x00\x08\x00\x01\xB0\x08"); }
    static void actuate(unsigned int dev, Smart_Data<SSB_Substation_Average_Power_Factor> * data, const Smart_Data<SSB_Substation_Average_Power_Factor>::Value & command) {}
};


typedef Smart_Data<SSB_Substation_Current> SSB_Current;
typedef Smart_Data<SSB_Substation_Voltage_R> SSB_Voltage_R;
typedef Smart_Data<SSB_Substation_Voltage_S> SSB_Voltage_S;
typedef Smart_Data<SSB_Substation_Voltage_T> SSB_Voltage_T;
typedef Smart_Data<SSB_Substation_Active_Power> SSB_Active_Power;
typedef Smart_Data<SSB_Substation_Average_Power_Factor> SSB_Average_Power_Factor;

typedef Smart_Data<Active_Power_Sensor> Active_Power;
typedef Smart_Data<ADC_Sensor> Luminous_Intensity;
typedef Smart_Data<Temperature_Sensor> Temperature;
typedef Smart_Data<Water_Flow_Sensor> Water_Flow;
typedef Smart_Data<Water_Flow_Sensor_WSTAR> Water_Flow_WSTAR;
typedef Smart_Data<Water_Flow_Sensor_M170> Water_Flow_M170;
typedef Smart_Data<Water_Level_Sensor> Water_Level;
typedef Smart_Data<Pressure_Sensor_Keller_Capacitive> Pressure_Keller_Capacitive;
typedef Smart_Data<Water_Level_Sensor_Ultrasonic_Microflex> Water_Level_Ultrasonic_Microflex;
typedef Smart_Data<Water_Turbidity_Sensor> Water_Turbidity;
typedef Smart_Data<Water_Turbidity_Sensor_Solar> Water_Turbidity_Solar;
typedef Smart_Data<Water_Turbidity_Sensor_OBS> Water_Turbidity_OBS;

typedef Smart_Data<Pluviometer> Rain;
typedef Smart_Data<RFID_Sensor> RFID;
typedef Smart_Data<Switch_Sensor> Presence;
typedef Smart_Data<Switch_Sensor> Switch;
typedef Smart_Data<Distance_Sensor> Distance;
typedef Smart_Data<Vibration_Sensor> Vibration;
typedef Smart_Data<Sound_Sensor> Sound;

typedef Smart_Data<Dummy_Transducer<TSTP::Unit::Temperature>> Temperature_Demonstration;


#endif

typedef Smart_Data<Keyboard_Sensor> Acceleration;

__END_SYS

#include "transducer.cc" // Static attributes go here

#endif
