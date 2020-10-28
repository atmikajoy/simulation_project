#include"vehicle.h"
#include "fuel.h"
#include<iostream>
#include "driver.h"
#include "../tee/thread_pool.h"

int main()
{
	/*{
		using namespace atmik::simulation;
		vehicle v(vehicle::THREE_WHEEL, fuel::PETROL, 1);
		for (int i = 0; i < 100; ++i)
		{
			v = v.next_vehicle();
			static const std::string type_string[]{ "CAR", "HEAVY", "2-wheeler", "3-wheeler" };
			std::cout << v.id << ' ' << type_string[v.type] << ' ' 
				      << to_string(v.fuel_type) << ' ' << v.arrival_time << '\n';
		}
		return 0;
	}*/
	{
		atmik::simulation::driver d(0, 3000);
		d.run();
		atmik::util::thread_pool::default_pool().stop();
		return 0;
	}
	for (int i = 0; i < 100; ++i)
	{
		using namespace atmik::simulation;
		vehicle i10(vehicle::CAR, fuel::PETROL);
		std::cout << "fuel required by the 10 is : " << i10.fuel_required_litres()
			<< "\nthe i10 is a :" << i10.to_string()
			<< "\nExtra service time(if serviced) :" << i10.extra_service_time_seconds() << '\n'
			<< '\n';

	}
}