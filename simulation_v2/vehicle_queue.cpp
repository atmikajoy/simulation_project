#include "vehicle_queue.h"
#include <stdexcept>

namespace atmik
{
	namespace simulation
	{
		bool vehicle_queue::enqueue(unsigned int vehicle_id)
		{
			if (pending_vehicles.size() == max_sz) return false;

			pending_vehicles.push(vehicle_id);
			return true;
		}

		vehicle& vehicle_queue::dequeue() // TO DO: look up id and return reference to vehicle
		{
			if (pending_vehicles.empty()) throw std::out_of_range("pop from empty queue");
			const auto id = pending_vehicles.front();
			pending_vehicles.pop();
			// return vehicle::look_up(id);
		}
	}
}
