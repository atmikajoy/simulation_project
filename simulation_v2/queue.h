#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include"vehicle.h"
#include <queue>
#include <cassert>
#include <stdexcept>

namespace atmik
{
	namespace simulation
	{
		struct vehicle_queue
		{

			vehicle_queue(std::size_t max_sz = 6) : max_sz(max_sz) {}

			bool enqueue(unsigned int vehicle_id)
			{
				assert(vehicle::exists(vehicle_id));
				if (!vehicle::exists(vehicle_id))
					throw std::logic_error("enqueue: invalid vehicle id");

				if( size() < max_size() )
				{
					pending_vehicles.push(vehicle_id);
					return true;
				}
				else return false; // queue full
			}

			unsigned int dequeue()
			{
				assert(!empty());
				if ( empty() )
					throw std::logic_error("dequeue: empty queue");

				unsigned int id = pending_vehicles.front();
				pending_vehicles.pop();
				return id;
			}

			bool empty() const { return pending_vehicles.empty(); }
			std::size_t size() const { return pending_vehicles.size(); }
			std::size_t max_size() const { return max_sz; }

		private:
			std::queue<unsigned int> pending_vehicles;
			std::size_t max_sz;
		};
	}
}

#endif // !QUEUE_H_INCLUDED

