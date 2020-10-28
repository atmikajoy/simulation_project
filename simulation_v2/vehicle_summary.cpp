#include"vehicle_summary.h"
#include <iostream>
#include "vehicle.h"
#include <map>
#include "service_bay.h"
#include "publisher.h"
#include"win_utils.h"
#include"log_dir.h"
#include<string>
#include<fstream>

namespace
{
	const std::string vehicle_summary_path = atmik::utils::log_dir() + "/vehicle_summary.txt";

	std::ostream& vehicle_log_stm()
	{
		static std::ofstream file(vehicle_summary_path);
		assert(file.is_open());
		return file;
	}

	struct vehicle_summary_stats
	{
		unsigned int num_arrived = 0;
		unsigned int num_serviced = 0;
		unsigned long long wait_time = 0;
		unsigned long long service_time = 0;
	};

	using fuel = atmik::simulation::fuel;
	using vtype = atmik::simulation::vehicle_type;
	std::map< std::pair<fuel, vtype>, vehicle_summary_stats > stats;

	void vehicle_arrival_listener(atmik::simulation::event e)
	{
		using namespace atmik::simulation;
		assert(e.type == event::event_type::VEHICLE_ARRIVAL);
		const vehicle& v = vehicle::look_up(e.info);
		// std::cout << v << '\n';
		// std::cin.get();
		auto& s = stats[{ v.ftype, v.vtype }];
		++s.num_arrived;
		s.num_serviced += e.handled;
	}

	void service_end_listener(atmik::simulation::event e)
	{
		using namespace atmik::simulation;
		assert(e.type == event::event_type::END_SERVICE);
		const auto& bay = service_bay::look_up(e.info);
		const auto& v = vehicle::look_up(bay.vehicle_being_serviced());
		auto& s = stats[{ v.ftype, v.vtype }];
		s.wait_time += v.wait_time();
		s.service_time += v.service_time();
	}
}
void atmik::simulation::reports::connect_vehicle_summary_listeners()
{
	static auto& publisher = event_publisher::default_publisher();
	publisher.add_listener(event::VEHICLE_ARRIVAL, vehicle_arrival_listener );
	publisher.add_listener(event::END_SERVICE, service_end_listener);
}

static void writing_vehicle_summary(std::ostream& stm)
{
	stm << "\n----------------VEHICLE SUMMARY-----------------\n";
	for (const auto& [key, s] : stats)
	{
		if (s.num_arrived)
		{
			stm << to_string(key.first) << ' ' << to_string(key.second) << '\n'
				<< "\t num_arrived: " << s.num_arrived << '\n'
				<< "\tpercent_serviced: " << s.num_serviced * 100.0 / s.num_arrived << "%\n"
				<< "\taverage wait time: " << double(s.wait_time) / s.num_serviced << '\n'
				<< "\taverage service time: " << double(s.service_time) / s.num_serviced
				<< "\n\n";
			stm << std::flush;
		}
	}
	stm << "\n----------------END OF VEHICLE SUMMARY-----------------\n";
}

void atmik::simulation::reports::print_vehicle_arrival_summary()
{
	writing_vehicle_summary(vehicle_log_stm());
	atmik::utils::win::create_process(
		atmik::utils::text_viewer_program_name() + ' ' +
		vehicle_summary_path
	);
	writing_vehicle_summary(std::cout);
}

