#include "fuel_storage.h"
#include "config.h"
#include <string>
#include <algorithm>
#include <cassert>
#include "publisher.h"
#include <iostream>

const int atmik::simulation::fuel_storage::threshold 
      = atmik::config::get<int>("simulation.fixed_parameters.threshold_percent", 25);

namespace atmik
{
	namespace simulation
	{
		namespace
		{
			struct refuel_rates
			{
				double refuel_petrol;
				double refuel_diesel;
				double refuel_lpg;
			};
			std::map<fuel, double> default_rates{
				{fuel::PETROL,5},
				{fuel::DIESEL,3},
				{fuel::LPG, 7}
			};

			std::map<fuel, double>& get_fuel_rates()
			{
				const std::string prefix = "simulation.";
				default_rates[fuel::PETROL] = atmik::config::get<double>(prefix + "petrol.nozzle_flow_rate", default_rates[fuel::PETROL]);
				default_rates[fuel::DIESEL] = atmik::config::get<double>(prefix + "diesel.nozzle_flow_rate", default_rates[fuel::DIESEL]);
				default_rates[fuel::LPG] = atmik::config::get<double>(prefix + "LPG.nozzle_flow_rate", default_rates[fuel::LPG]);
				return default_rates;
			}
		}

	}
}

atmik::simulation::fuel_storage::fuel_storage(atmik::simulation::fuel ftype) : ftype(ftype)
{
	assert(threshold > 8 && threshold < 76); // sanity check

	const std::string prefix = "simulation." + to_string(ftype)+'.';
	fuel_rate = atmik::config::get<double>(prefix + "fuel_rate", 1.5);
	capacity = atmik::config::get<double>(prefix + "capacity", 1000);
	current_quantity = atmik::config::get<double>(prefix + "current_quantity", 500);
	std::size_t max_q_sz = atmik::config::get <std::size_t>(prefix + "max_queue_size", 6);
	queue = vehicle_queue(max_q_sz);

	std::size_t num_bays = atmik::config::get <std::size_t>(prefix + "num_bays", 6);
	assert(num_bays > 0);
	if (num_bays == 0) throw std::logic_error("at least one service bay is required");
	while (bays.size() < num_bays)
	{
		service_bay& bay = service_bay::create( id() );
		bays.push_back(bay.id());
	}
}

using namespace atmik::simulation;

bool atmik::simulation::fuel_storage::handle_end_service(event e)
{
	if (e.type != event::END_SERVICE || !has_bay(e.info)) return false;

	if (refuelling_pending())
	{
		for (auto id : bays)
			if (service_bay::look_up(id).is_occupied()) return false;

		refuel();
	}

	if (queue.empty()) return true; // nothing to be done

	return service_bay::look_up(e.info).start_service(queue.dequeue());
}

void atmik::simulation::fuel_storage::refuel()
{

	const auto qty = capacity - current_quantity;

	static auto& fuel_rates = get_fuel_rates();
	static const double refuel_rate = fuel_rates[ftype]; 
	const auto refuel_time = (unsigned int)std::round( qty/refuel_rate ) ;
	const unsigned int fuel_type_id = (unsigned int)ftype;
	event::add_event(event::TANK_REFILL_START, default_clock()() , 6, fuel_type_id);
	event::add_event(event::TANK_REFILL_END, default_clock()() + refuel_time, 7, fuel_type_id );
}

bool atmik::simulation::fuel_storage::handle_tank_refill_end(atmik::simulation::event e )
{
	if (e.info != (unsigned int)ftype) return false; // some other storage, not this one

	// std::cout << "*** storage " << to_string(ftype) << " tank refill ended at "
		// << default_clock() << ' '  << e.time << '\n';
	// std::cin.get();

	refuel_pending = false;
	current_quantity = capacity; // filled to capacity now
	
	for (auto bay_id : bays)
	{
		if (!queue.empty())
		{
			auto vehicle_id = queue.dequeue();
			service_bay::look_up(bay_id).start_service(vehicle_id);
		}
	}

	return true;
}

bool atmik::simulation::fuel_storage::handle_vehicle_arrival(event e)
{
	if (e.type != event::VEHICLE_ARRIVAL) return false;
	vehicle& v = vehicle::look_up(e.info);
	if (v.ftype != ftype) return false; // not our fuel type, do not handle

	return queue.enqueue(v.id()); // try to queue it
}


bool atmik::simulation::fuel_storage::has_bay(unsigned int bay_id) const
{
	return std::find(bays.begin(), bays.end(), bay_id) != bays.end();

}

void atmik::simulation::fuel_storage::connect_events()
{
	auto& publisher = event_publisher::default_publisher();
	static const auto arrival_handler = &fuel_storage::handle_vehicle_arrival;
	static const auto end_service_handler = &fuel_storage::handle_end_service;
	static const auto tank_refill_end_handler = &fuel_storage::handle_tank_refill_end;

	publisher.add_handler(event::VEHICLE_ARRIVAL, this, arrival_handler);
	publisher.add_handler(event::END_SERVICE, this, end_service_handler);
	publisher.add_handler(event::TANK_REFILL_END, this, tank_refill_end_handler);
}
