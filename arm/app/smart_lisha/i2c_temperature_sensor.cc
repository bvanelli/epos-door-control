#include <smart_data.h>
#include <alarm.h>
#include <i2c.h>

using namespace EPOS;

OStream cout;

// TODO: pass it to transducer.h
class Temperature_Transducer
{
public:
    // === Technicalities ===
    // We won't use these, but every Transducer must declare them
    typedef _UTIL::Observer Observer;
    typedef _UTIL::Observed Observed;

    typedef Dummy_Predictor Predictor;
    struct Predictor_Configuration : public Predictor::Configuration {};

    static void attach(Observer * obs) {}
    static void detach(Observer * obs) {}

public:
    // === Sensor characterization ===
    static const unsigned int UNIT = TSTP::Unit::Temperature;
    static const unsigned int NUM = TSTP::Unit::I32;
    static const int ERROR = 0; // Unknown

    static const bool INTERRUPT = false;
    static const bool POLLING = true;

public:
    // === SmartData interaction ===
    Temperature_Transducer() {}

    static void sense(unsigned int dev, Smart_Data<Temperature_Transducer> * data) {
        cout << "sense(dev=" << dev << ")" << endl;
        data->_value = sensor.get();
    }

    static void actuate(unsigned int dev, Smart_Data<Temperature_Transducer> * data, const Smart_Data<Temperature_Transducer>::Value & command) {
        cout << "actuate(dev=" << dev << ", command=" << command << ")" << endl;
        data->_value = command;
    }

    static I2C_Temperature_Sensor sensor;
};

// A SmartData that personifies the transducer defined above
I2C_Temperature_Sensor Temperature_Transducer::sensor;
typedef Smart_Data<Temperature_Transducer> My_Temperature;

int main()
{
    Alarm::delay(5000000);
    cout << "Hello!" << endl;

    // My_Temperature t(0, 5000000, My_Temperature::ADVERTISED);
    My_Temperature t(0, 5000000, My_Temperature::PRIVATE);
    //Thread::self()->suspend();
    while(true)
    {
        Alarm::delay(5000000);
        cout << "Temperature = " << t << " at " << t.location() << ", " << t.time() << endl;
    }
    return 0;
}