#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include"vehicle.h"
#include<queue>

namespace atmik
{
	namespace simulation
	{
		struct queue_v
		{
			
			queue_v(std::size_t max_sz = 6) : max_sz(max_sz) {}
			bool enqueue(vehicle v);
			vehicle dequeue(); 
			bool empty() { return pending_vehicles.empty(); }
			std::size_t size();
			std::size_t max_sz_() { return max_sz;  }

		private:
			std::queue<vehicle> pending_vehicles; 
			std::size_t max_sz; 
		};
	}
}

#endif // !QUEUE_H_INCLUDED

