#include"vehicle_type.h"
#include"config.h"
#include<string.h>
#include<random>
#include <cmath>
#include <algorithm>
#include<map>
#include "fuel.h"
namespace
{
	const std::string key_prefix = "simulation.vehicles.";
	std::mt19937 rng(std::random_device{}());

	struct arrival_rate
	{
		unsigned int mean_arrival_rate; 
	};

	struct fuel_required
	{
		double min_fuel; 
		double max_fuel;
		double mean_fuel;
		double sd_fuel; 
	};

	struct fuel_select
	{
		unsigned int pct_petrol;
		unsigned int pct_diesel;
		unsigned int pct_lpg;
	};

	struct extra_service
	{
		double percent; 
		unsigned min_time;
		unsigned max_time;
	};

	using v = atmik::simulation::vehicle_type;
	std::map<v, arrival_rate> arrival_data = {
		{v::CAR , {35}},
		{v::HEAVY_VEHICLE, {20}},
		{v::THREE_WHEEELER, {40}},
		{v::TWO_WHEELER, {25}}

	};
	std::map<v, fuel_required> fuel_data = {
		{v::CAR, {1,50,20,5}},
		{v::HEAVY_VEHICLE, {1,300,120,30}},
		{v::TWO_WHEELER,{1,7,2,1}},
		{v::THREE_WHEEELER,{1,10,7,1}}
	};
	std::map<v, fuel_select> selection_data
	{
		{v::CAR, {65,35,0}},
		{v::HEAVY_VEHICLE,{0,100,0}},
		{v::TWO_WHEELER,{100,0,0}},
		{v::THREE_WHEEELER,{0,0,100}}
	};
	std::map<v, extra_service> extra_service_data
	{
		{v::CAR,{15,30,180}},
		{v::HEAVY_VEHICLE,{25,30,300}},
		{v::TWO_WHEELER,{10,30,120}},
		{v::THREE_WHEEELER,{20,30,300}}
	};

	int get_data_xml()
	{
		for (auto vt : { v::CAR,v::HEAVY_VEHICLE,v::THREE_WHEEELER,v::TWO_WHEELER })
		{
			std::string p = key_prefix + to_string(vt);
			arrival_data[vt].mean_arrival_rate = atmik::config::get<unsigned int>(p + ".mean_arrivals_per_hour", arrival_data[vt].mean_arrival_rate);

			selection_data[vt].pct_petrol = atmik::config::get<unsigned int>(p + ".fuel_type_percentages.petrol", selection_data[vt].pct_petrol);
			selection_data[vt].pct_diesel = atmik::config::get<unsigned int>(p + ".fuel_type_percentages.diesel", selection_data[vt].pct_diesel);
			selection_data[vt].pct_lpg = atmik::config::get<unsigned int>(p + ".fuel_type_percentages.lpg", selection_data[vt].pct_lpg);

			fuel_data[vt].min_fuel = atmik::config::get<double>(p + ".fuel_amounts.min", fuel_data[vt].min_fuel);
			fuel_data[vt].max_fuel = atmik::config::get<double>(p + ".fuel_amounts.max", fuel_data[vt].max_fuel);
			fuel_data[vt].mean_fuel = atmik::config::get<double>(p + ".fuel_amounts.mean", fuel_data[vt].mean_fuel);
			fuel_data[vt].sd_fuel = atmik::config::get<double>(p + ".fuel_amounts.sd", fuel_data[vt].sd_fuel);

			extra_service_data[vt].percent = atmik::config::get<double>(p + ".extra_service.percent", extra_service_data[vt].percent);
			extra_service_data[vt].max_time = atmik::config::get<unsigned int>(p + ".extra_service.max_time", extra_service_data[vt].max_time);
			extra_service_data[vt].min_time = atmik::config::get<unsigned int>(p + ".extra_service.min_time", extra_service_data[vt].min_time);
		}
		return 1;
	}

	int i = get_data_xml();
}

namespace atmik
{
	namespace simulation
	{
		int arrival_rate(vehicle_type vt)
		{
			auto iter = arrival_data.find(vt);
			auto rate = iter != arrival_data.end() ? iter->second.mean_arrival_rate : 0 ;
			return rate;
		}

		fuel select_fuel(vehicle_type vt)
		{
			static std::uniform_int_distribution<int> distrib(1, 100);
			unsigned int r = distrib(rng);
			if (r <= selection_data.find(vt)->second.pct_petrol) return fuel::PETROL;
			r -= selection_data.find(vt)->second.pct_petrol;
			return r <= selection_data.find(vt)->second.pct_diesel ? fuel::DIESEL : fuel::LPG;
		}

		double fuel_required(vehicle_type vt)
		{
			const auto vehicle_prefix = key_prefix + to_string(vt) + ".fuel_amounts.";
			auto fd = fuel_data.find(vt)->second;
			std::normal_distribution<double> distrib(fd.mean_fuel, fd.sd_fuel);
			return std::clamp(distrib(rng), fd.min_fuel, fd.max_fuel);
		}

		int extra_service_time(vehicle_type vt)
		{
			std::uniform_int_distribution<int> distrib(0, 100);

			const auto v = extra_service_data.find(vt)->second;
			const auto n = distrib(rng);
			if (n > 0 && n <= v.percent)
			{
				const double serv_time = std::uniform_real_distribution<double>(v.min_time, v.max_time)(rng);
				return int(std::round(serv_time));
			}
			return 0;
		}
	}
}