#include "service_bay.h"
#include<random>
#include "../config/config.h"
#include<algorithm>
#include "fuel_storage.h"
#include <cassert>
#include <stdexcept>

int atmik::simulation::service_bay::next_bay_id;

std::string atmik::simulation::service_bay::descr() const
{
	return to_string(storage->fuel_type) + " #" + std::to_string(id);
}

atmik::simulation::service_bay::service_bay(fuel_storage* storage) : storage(storage), occupied(false)
{
	assert(storage != nullptr);
	if (storage == nullptr) throw std::invalid_argument("nullptr");
}

//static int bay_id = 0; 
void atmik::simulation::service_bay::start_service(unsigned int start_time, atmik::simulation::vehicle v, int bay)
{
	last_serviced = v;
	last_serviced.start_service = start_time;
	std::mt19937 rng(std::random_device{}());
	auto fuel_required = v.fuel_required_litres();
	int time_taken = int( std::round( fuel_required / storage->fuel_rate ) );
	service_end_at = start_time + time_taken;
	last_serviced.end_service = end_service_time();
	occupied = true; 
	
}

unsigned int atmik::simulation::service_bay::end_service_time() const
{
	return service_end_at;
}

void atmik::simulation::service_bay::end_service()
{
	occupied = false; 
	service_end_at = -1; 
	last_serviced = {};
}