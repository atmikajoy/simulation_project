#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

#include "clock.h"

namespace atmik
{
	namespace simulation
	{
		namespace driver
		{
			void run(clock::time_of_day_t start_time, clock::time_of_day_t end_time,
				unsigned int ndays);

			void run(clock::time_of_day_t start_time, unsigned int duration_secs_per_day,
				unsigned int ndays);
		}
	}
}

#endif // DRIVER_H_INCLUDED