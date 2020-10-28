#ifndef FUEL_STORAGE_H_INCLUDED
#define FUEL_STORAGE_H_INCLUDED

#include "fuel.h"
#include "queue.h"
#include "service_bay.h"
#include "object_with_unique_id.h"

namespace atmik
{
	namespace simulation
	{
		struct fuel_storage : atmik::util::object_with_unique_id<fuel_storage>
		{
			fuel_storage(fuel ftype);

			fuel fuel_type() const { return ftype;  }
			double f_rate() const { return fuel_rate; }

			bool handle_end_service(event e);
			bool handle_vehicle_arrival(event e);

			bool has_bay(unsigned int bay_id) const;

			void connect_events();

			double current_quantity;

			double capacity_tank() { return capacity; }

			bool refuelling_pending() const { return refuel_pending; }

			void fuel_amount_sold(double amt)
			{
				current_quantity -= amt;

				if ( current_quantity < capacity / (100.0/threshold) )
				{
					refuel_pending = true;
				}
			}

			void refuel();

			bool handle_tank_refill_end( event e );
			// bool handle_end_of_day(event e);

			double fuel_remaining_in_tank() const { return capacity - current_quantity;  }

			static fuel_storage& look_up_fuel(fuel f)
			{
				for (auto& [id, fs] : objects())
					if (fs.ftype == f) return fs;

				throw std::logic_error("bad fuel type");

			}

		private:

			double fuel_rate;
			double capacity;
			fuel ftype; 
			vehicle_queue queue; 
			std::vector<unsigned int> bays;
			bool refuel_pending = false;

			static const int threshold; // percent (threshold for refuelling)
		};
	}
}


#endif // !FUEL_STORAGE_H_INCLUDED
