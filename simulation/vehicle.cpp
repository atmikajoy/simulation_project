#include"vehicle.h"
#include"../config/config.h"
#include <map>
#include<random>
#include<algorithm>
#include <cassert>

namespace atmik
{
	namespace simulation
	{
		namespace
		{
			static std::mt19937 rng(std::random_device{}());

			struct data
			{
				double min_fuel;
				double max_fuel;
				double mean_fuel;
				double sd_fuel;

				int percent_extra_service_reqd; 
				int min_service_time_secs;
				int max_service_time_secs;

				int mean_arrivals_per_hour;
				int percent_petrol_driven;
			};

			using data_map = std::map< vehicle::v_type, data >;
			
			const data_map default_values
			{
				{ vehicle::CAR, { 1, 50, 20, 5, 15, 30, 180, 20 , 65} },
				{vehicle::HEAVY , {1,300, 120, 30, 25,30,300,10, 0 }},
				{vehicle::TWO_WHEEL , {1,7, 2, 0.5,10,330,120,40, 100 }},
				{vehicle::THREE_WHEEL , {1,10, 7, 1, 20,30,300,25, 0 }},

			};
			
			data_map make_data_map() // initialise by reading from xml file
			{
				data_map map;
				for (vehicle::v_type type : { vehicle::CAR, vehicle::HEAVY, vehicle::THREE_WHEEL, vehicle::TWO_WHEEL })
				{
					assert(default_values.find(type) != default_values.end());
					const data& def_data = default_values.find(type)->second;

				    std::string type_str ;
					switch (type)
					{
					case vehicle::CAR: type_str = "car"; break;
						case vehicle::HEAVY: type_str = "heavy"; break;
						case vehicle::TWO_WHEEL: type_str = "2-wheeler"; break;
						case vehicle::THREE_WHEEL: type_str = "3-wheeler"; break;
					}

					data v_data{};

					v_data.min_fuel = atmik::config::get<double>("simulation.vehicles." + type_str + ".min_fuel", def_data.min_fuel);
					v_data.max_fuel = atmik::config::get<double>("simulation.vehicles." + type_str + ".max_fuel", def_data.max_fuel);
					assert(v_data.min_fuel >= 0 && v_data.min_fuel <= v_data.max_fuel);
				    	
					v_data.mean_fuel = atmik::config::get<double>("simulation.vehicles." + type_str + ".mean_fuel", def_data.mean_fuel);
					v_data.sd_fuel = atmik::config::get<double>("simulation.vehicles." + type_str + ".sd_fuel", def_data.sd_fuel);

					v_data.percent_extra_service_reqd = atmik::config::get<int>("simulation.vehicles." + type_str + ".percent_extra_service_reqd", def_data.percent_extra_service_reqd);
					v_data.min_service_time_secs = atmik::config::get<int>("simulation.vehicles." + type_str + ".min_service_time_secs", def_data.min_service_time_secs);
					v_data.max_service_time_secs = atmik::config::get<int>("simulation.vehicles." + type_str + ".max_service_time_secs", def_data.max_service_time_secs);

					v_data.mean_arrivals_per_hour = atmik::config::get<int>("simulation.vehicles." + type_str + ".mean_arrivals_per_hour", def_data.mean_arrivals_per_hour);
					v_data.percent_petrol_driven = atmik::config::get<int>("simulation.vehicles." + type_str + ".percent_petrol_driven", def_data.percent_petrol_driven);
				
					map[type] = v_data;
				}
				return map;
			}

			const data_map actual_values = make_data_map();

		}
	}
}

std::string atmik::simulation::vehicle::type_string() const
{
	switch (type)
	{
		case CAR: return "car";
		case HEAVY: return "heavy";
		case TWO_WHEEL: return "2-wheeler";
		case THREE_WHEEL: return "3-wheeler"; 
	}

	return "";
}

std::string atmik::simulation::vehicle::to_string() const
{
	return atmik::simulation::to_string(fuel_type) + ' ' + type_string() ;
}

double atmik::simulation::vehicle::compute_fuel_required_litres() const
{
	

	assert(actual_values.find(type) != actual_values.end());
	const data& v = actual_values.find(type)->second;

	std::normal_distribution<double> distrib(v.mean_fuel, v.sd_fuel);
	return std::clamp(distrib(rng), v.min_fuel, v.max_fuel);
}

int atmik::simulation::vehicle::extra_service_time_seconds() const
{
	std::uniform_int_distribution<int> distrib(0, 100);
	
	const data& v = actual_values.find(type)->second;
	const auto n = distrib(rng);
	if ( n > 0 && n <= v.percent_extra_service_reqd)
	{
		const double serv_time = std::uniform_real_distribution<double>(v.min_service_time_secs, v.max_service_time_secs)(rng);
		return int(std::round(serv_time));
	}
	return 0; 

}

atmik::simulation::vehicle atmik::simulation::vehicle::next_vehicle() const
{
	static std::random_device rd;
	static std::mt19937 rng(rd());
	const data& v = actual_values.find(type)->second;
	static std::exponential_distribution<> distrib(v.mean_arrivals_per_hour);

	const unsigned int inter_arrival_time_secs = int( std::round(distrib(rng) * 3600) );

	fuel f = fuel::PETROL;
	if (std::uniform_int_distribution<int>(0, 100)(rng) > v.percent_petrol_driven && this->type == CAR)
		f = fuel::DIESEL;
	if (this->type == HEAVY ) f = fuel::DIESEL;
	if (this->type == TWO_WHEEL) f = fuel::PETROL;
	if (this->type == THREE_WHEEL) f = fuel::LPG;

	return vehicle(type, f,last_arrival_time[type] + inter_arrival_time_secs );
}


std::ostream& operator << (std::ostream& stm, const atmik::simulation::vehicle& v)
{
	stm << "vehicle details";
	return stm;
}

int atmik::simulation::vehicle::vehicle_id = 0;

unsigned int atmik::simulation::vehicle::last_arrival_time[atmik::simulation::vehicle::NTYPES];