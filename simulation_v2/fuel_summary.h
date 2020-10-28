#ifndef FUEL_SUMMARY_H_INCLUDED
#define FUEL_SUMMARY_H_INCLUDED

#include "event.h"
#include "fuel.h"
#include "vehicle.h"
#include<iostream>
#include<fstream>

namespace atmik
{
	namespace simulation
	{
		namespace reports
		{
			void fuel_summary_listener(event e);

			void print_fuel_summary(); 

			void overall_tank_refill_listener(event e);


		}
	}
}



#endif // !FUEL_SUMMARY_H_INCLUDED
