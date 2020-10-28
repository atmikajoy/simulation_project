#ifndef SERVICE_BAY_H_INCLUDED
#define SERVICE_BAY_H_INCLUDED

#include "object_with_unique_id.h"
#include "vehicle_type.h"
#include "vehicle.h"
#include "event.h"

namespace atmik
{
	namespace simulation
	{
		struct fuel_storage;

		struct service_bay : atmik::util::object_with_unique_id<service_bay,500>
		{
			service_bay( unsigned int fuel_storage_id ) : fuel_storage_id(fuel_storage_id) {}

			fuel_storage& storage() const;  

			bool start_service( unsigned int vehicle_id );// event handle for start

			unsigned int end_service_time() const { return service_end_at; }

			bool handle_end_service( event e ); 

			bool is_occupied() const { return occupied; }

			unsigned int vehicle_being_serviced() const { return current_vehicle_id; }

			bool handle_vehicle_arrival(event e);

			bool matching_fuel_type(unsigned int vehicle_id) const;

			bool handle_fuel_refill(event e);

			bool refill_start();

			fuel fuel_type() const;

			static void connect_events();

		private:

			unsigned int fuel_storage_id ;
			bool occupied = false;
			unsigned int service_end_at = -1;
			unsigned int current_vehicle_id = -1;
		};
	}
}



#endif // !SERVICE_BAY_H_INCLUDED
