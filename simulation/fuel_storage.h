#ifndef FUEL_STORAGE_H_INCLUDED
#define FUEL_STORAGE_H_INCLUDED

#include "fuel.h"
#include "service_bay.h"
#include <vector>
#include <cassert>
#include"queue.h"
#include <cmath>

namespace atmik
{
	namespace simulation
	{
		struct fuel_storage
		{
			fuel_storage(fuel fuel_type);

			std::size_t size() { return bays.size(); }

			fuel_storage& deduct_fuel(double fuel_used) { capacity -= fuel_used; return *this; }

			unsigned int refilling_time() const { return (unsigned int) std::round( capacity / 100 ) ;  }
			
			fuel fuel_type; 
			double fuel_rate;
			std::vector<service_bay> bays; //  std::size_t num_bays;
			double capacity;
			double current_quantity;
			queue_v pending;// queue of his max size // std::size_t max_queue_size;

			
		};
	}
}




#endif // !FUEL_STORAGE_H_INCLUDED
