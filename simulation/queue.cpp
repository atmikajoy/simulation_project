#include "queue.h"
#include<stdexcept>

bool atmik::simulation::queue_v::enqueue(atmik::simulation::vehicle v)
{
	if (pending_vehicles.size() < max_sz)
	{
		pending_vehicles.push(v);
		return true;
	}
	return false; 
}
atmik::simulation::vehicle atmik::simulation::queue_v::dequeue()
{
	if (!pending_vehicles.empty())
	{
		auto popped_vehicle = pending_vehicles.front();
		pending_vehicles.pop();
		return popped_vehicle;
	}
	else throw std::domain_error("queue is empty");
}
std::size_t atmik::simulation::queue_v::size()
{
	return pending_vehicles.size(); 
}