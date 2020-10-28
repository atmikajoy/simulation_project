#ifndef VEHICLE_SUMMARY_H_INCLUDED
#define VEHICLE_SUMMARY_H_INCLUDED

#include"vehicle.h"
#include <queue>
#include <cassert>
#include <stdexcept>
#include "event.h"
#include <cassert>

namespace atmik
{
	namespace simulation
	{
		namespace reports
		{

			void connect_vehicle_summary_listeners();

			void print_vehicle_arrival_summary();

		}
	}
}

#endif // VEHICLE_SUMMARY_H_INCLUDED
