#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

#include"event.h"
#include"clock.h"
#include"service_bay.h"
#include"vehicle.h"
#include"fuel.h"
#include"fuel_storage.h"
#include"queue.h"
#include<iostream>

namespace atmik
{
	namespace simulation
	{
		struct driver
		{
			driver(unsigned int start_simul, unsigned int end_simul): 
				start_simul(start_simul), end_simul(end_simul) 
			{
				event::add_event(event::START_SIMULATION, start_simul, 0);
				event::add_event(event::END_SIMULATION, end_simul,0);
			}

			

			void run();
			
			
			unsigned int start_simul;
			unsigned int end_simul;
			static clock clk;
		};
	}
}
#endif // !DRIVER_H_INCLUDED
