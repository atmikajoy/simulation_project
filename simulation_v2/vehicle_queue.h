#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include<queue>

namespace atmik
{
	namespace simulation
	{
		struct vehicle; // just for now

		struct vehicle_queue
		{

			vehicle_queue(std::size_t max_sz = 6) : max_sz(max_sz) {}
			bool enqueue(unsigned int vehicle_id );
			vehicle& dequeue(); // TO DO: look up id and return reference to vehicle
			bool empty() { return pending_vehicles.empty(); }
			std::size_t size() const { return pending_vehicles.size(); }
			std::size_t max_sz_() const { return max_sz; }

		private:
			std::queue<unsigned int> pending_vehicles;
			std::size_t max_sz;
		};
	}
}

#endif // !QUEUE_H_INCLUDED

