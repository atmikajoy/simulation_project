#ifndef VEHICLE_H_INCLUDED
#define VEHICLE_H_INCLUDED

#include "vehicle_type.h"
#include "fuel.h"
#include "object_with_unique_id.h"
#include "clock.h"
#include <iosfwd>
#include "event.h"

namespace atmik
{
	namespace simulation
	{
		// vehicle ids start at 10001
		struct vehicle : atmik::util::object_with_unique_id<vehicle,10000>
		{
			using base = atmik::util::object_with_unique_id<vehicle, 10000>;

			vehicle(unsigned int arrival_time, vehicle_type type);

			static vehicle& next_arrival(vehicle_type type,
				unsigned int last_arrival_time = default_clock()());

			unsigned int wait_time() const { return start_service - arrival_time;  }
			unsigned int service_time() const { return end_service - start_service; }

			static void schedulr_first_vehicles();
			static bool handle_start_of_day( event e );
			static bool handle_end_of_day(event e);

			fuel ftype = fuel::PETROL;
			vehicle_type vtype = vehicle_type::CAR;

			unsigned int arrival_time = 0; 
			unsigned int start_service = -1; 
			unsigned int end_service = -1;

			double fuel_required = 0 ;
			int extra_service_time = 0;

			static bool handle_arrival(event e);

			static void connect_events();

			static bool no_more_vehicles;
		};

		std::ostream& operator<< (std::ostream&, const vehicle&);
	}
}


#endif // !VEHICLE_H_INCLUDED
