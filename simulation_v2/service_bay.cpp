#include "service_bay.h"
#include "vehicle.h"
#include "fuel_storage.h"
#include "publisher.h"
#include "clock.h"
#include "config.h"
#include <random> 

namespace atmik
{
	namespace simulation
	{
		namespace
		{

			const unsigned int min_setup_time = atmik::config::get<int>("simulation.fixed_parameters.min_setup_time",30);
			const unsigned int max_setup_time = atmik::config::get<int>("simulation.fixed_parameters.max_setup_time", 60);
			//static_assert(max_setup_time >= min_setup_time);
			std::uniform_int_distribution<unsigned int> setup_distrib(min_setup_time, max_setup_time);
			std::mt19937 rng(std::random_device{}());
		}
		fuel service_bay::fuel_type() const
		{
			return storage().fuel_type();
		}

		fuel_storage& service_bay::storage() const
		{
			return fuel_storage::look_up(fuel_storage_id);
		}

		bool service_bay::start_service(unsigned int vehicle_id)
		{
			if (occupied ) return false;
			if (storage().refuelling_pending())
			{
				occupied = false;
				return false; // refulelling pending, pause servicing
			}

			auto& v = vehicle::look_up(vehicle_id);
			v.start_service = default_clock()();
			unsigned int time_taken =int( std::round(v.fuel_required / storage().f_rate()));
			time_taken += v.extra_service_time;
			time_taken += setup_distrib(rng);
			service_end_at = v.start_service + time_taken;
			v.end_service = end_service_time();
			occupied = true;
			current_vehicle_id = v.id();
			storage().fuel_amount_sold(v.fuel_required);
			event::add_event(event::START_SERVICE, v.start_service, 20, id());
			event::add_event(event::END_SERVICE, v.end_service, 21, id());
			return true;
		}

		bool service_bay::handle_end_service( event e )
		{
			// wrong event ype (sanity check) or not this bay; can't handle it
			if (e.type != event::END_SERVICE || e.info != id()) return false;

			occupied = false;
			// current_vehicle_id = -1;
			service_end_at = -1;

			// the event is not completely handled, so we need to propagate it 
			// to the handler in the fuel_storage. which would then deque a vehicle 
			// and call our start_service. so we return false (partially handled)
			return false; 
		}

		bool service_bay::handle_vehicle_arrival(event e)
		{
			// bay not free or wrong fuel type; can't handle it
			if (!matching_fuel_type(e.info)) return false;

			return start_service(e.info);
		}

		bool service_bay::matching_fuel_type(unsigned int vehicle_id) const
		{
			return vehicle::look_up(vehicle_id).ftype == fuel_type();
		}

		void service_bay::connect_events()
		{
			auto& publisher = event_publisher::default_publisher();
			static const auto arrival_handler = &service_bay::handle_vehicle_arrival;
			static const auto end_service_handler = &service_bay::handle_end_service;
			for (auto& [id, bay] : objects() )
			{
				publisher.add_handler(event::VEHICLE_ARRIVAL, std::addressof(bay), arrival_handler);
				publisher.add_handler(event::END_SERVICE, std::addressof(bay), end_service_handler );
			}
		
		}
	}
}
