#include "vehicle.h"
#include <random>
#include <algorithm>
#include <map>
#include <iostream>
#include "publisher.h"


namespace atmik
{
	namespace simulation
	{
		namespace
		{
			std::mt19937 rng(std::random_device{}());

			std::map< vehicle_type, std::exponential_distribution<double> >& distrib()
			{
				static std::map< vehicle_type, std::exponential_distribution<double> > distribution_map
				{
					{
						vehicle_type::CAR,
						std::exponential_distribution<double>(arrival_rate(vehicle_type::CAR))
					},

					{
						vehicle_type::HEAVY_VEHICLE,
						std::exponential_distribution<double>(arrival_rate(vehicle_type::HEAVY_VEHICLE))
					},

					{
						vehicle_type::THREE_WHEEELER,
						std::exponential_distribution<double>(arrival_rate(vehicle_type::THREE_WHEEELER))
					},

					{
						vehicle_type::TWO_WHEELER,
						std::exponential_distribution<double>(arrival_rate(vehicle_type::TWO_WHEELER))
					},

				};

				return distribution_map;
			}
		}

		bool vehicle::no_more_vehicles = false;

		vehicle::vehicle(unsigned int arrival_time, vehicle_type type)
			: arrival_time(arrival_time), vtype(type)
		{
			ftype = select_fuel(vtype);
			fuel_required = atmik::simulation::fuel_required(vtype);
			extra_service_time = atmik::simulation::extra_service_time(vtype);
		}

		vehicle& vehicle::next_arrival(vehicle_type type, unsigned int last_arrival_time )
		{
			auto& d = distrib()[type];
			// note:: inter-arrival-time in hours converted to nearest seconds
			unsigned int inter_arrival_time = (unsigned int)std::round( d(rng) * 3600 );
			vehicle& v = base::create(last_arrival_time + inter_arrival_time, type);

			if( no_more_vehicles == false )
			    event::add_event(event::VEHICLE_ARRIVAL, last_arrival_time + inter_arrival_time, 10, v.id());

			// std::cerr << v.id() << ' ' << std::flush; // *** temporary
			
			// the driver is responsible for pulling and publishing events
			// event_publisher::default_publisher().publish(
				// event(event::VEHICLE_ARRIVAL, v.arrival_time, 10, v.id()));
			
			return v;
		}

		std::ostream& operator<< (std::ostream& stm, const vehicle& v )
		{
			stm << 'V' << v.id() << ' ' << to_string(v.vtype) << ' ' << to_string(v.ftype)
				<< ' ' << v.arrival_time;
			if (v.start_service != -1) stm << ' ' << v.start_service;
			if (v.end_service != -1) stm << ' ' << v.end_service;
			return stm;
		}

		bool vehicle::handle_arrival(event e)
		{
			// if (e.info > 10200) return false; // temporaru

			if (no_more_vehicles) return false;

			if (e.type == event::VEHICLE_ARRIVAL)
				next_arrival(look_up(e.info).vtype, e.time);

			return false; // allow propagation to other handlers
		}

		void vehicle::connect_events()
		{
			event_publisher::default_publisher().add_handler(event::VEHICLE_ARRIVAL, handle_arrival);
			event_publisher::default_publisher().add_handler(event::START_OF_DAY, handle_start_of_day);
			event_publisher::default_publisher().add_handler(event::END_OF_DAY, handle_end_of_day);
		}

		void vehicle::schedulr_first_vehicles()
		{
			for (auto vt : { vehicle_type::CAR, vehicle_type::HEAVY_VEHICLE,
							 vehicle_type::THREE_WHEEELER, vehicle_type::TWO_WHEELER })
				next_arrival(vt);
		}

		bool vehicle::handle_start_of_day( event e )
		{
			no_more_vehicles = false;
			schedulr_first_vehicles();
			return true;
		}

		bool vehicle::handle_end_of_day(event e)
		{
			no_more_vehicles = true;
			return true;
		}
	}

}
