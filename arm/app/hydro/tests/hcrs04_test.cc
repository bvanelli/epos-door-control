#include <transducer.h>
#include <thread.h>
#include <smart_data.h>

using namespace EPOS;

OStream cout;

int main()
{
	GPIO relay('B', 3, GPIO::OUT); //relay P6 in hydro board
	relay.clear();
    GPIO trigger('A', 1, GPIO::OUT);
    GPIO echo('A', 5, GPIO::IN); //signal from P6 in hydro board
    Distance_Sensor distance_sensor(0, &trigger, &echo);
    Distance distance(0, 1000000, Distance::PRIVATE);
    while(true)
    {
    	Delay(5000000);
    	relay.set();
    	Delay(10000000);
    	cout << "Distance: " << distance << endl;
    	relay.clear();
    }

    return 0;
}

