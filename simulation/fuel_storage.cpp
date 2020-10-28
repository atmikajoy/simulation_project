#include "fuel_storage.h"
#include <map> 
#include"../config/config.h"
#include <stdexcept>
#include <cassert>
#include<vector>
namespace atmik
{
	namespace simulation
	{
		namespace
		{
			struct fuel_data
			{
				double fuel_rate; 
				std::size_t num_bays; 
				double capacity; 
				double current_quantity;
				std::size_t max_queue_size;

			};

			using fuel_map = std::map<fuel, fuel_data>;


			fuel_map get_fuel_data()
			{
				static fuel_map default_values =
				{
					{fuel::PETROL , {9.5,3,1000,500,6}},
					{fuel::DIESEL, {10,3,1000, 500,6}},
					{fuel::LPG, {5, 2, 500, 250,6}}
				};

				fuel_map actual_values; 

				for (auto ftype : { fuel::PETROL, fuel::DIESEL, fuel::LPG })
				{
					fuel_data f_data = default_values[ftype];
					const std::string prefix = "simulation.storage." + to_string(ftype);
					auto& ad = actual_values[ftype];
					
					ad.fuel_rate = atmik::config::get<double>(prefix + ".fuel_rate", f_data.fuel_rate);
					ad.num_bays = atmik::config::get<std::size_t>(prefix + ".num_bays", f_data.num_bays);

					ad.capacity = atmik::config::get<double>(prefix + ".capacity", f_data.capacity);
					ad.current_quantity = atmik::config::get<double>(prefix + ".current_quantity", f_data.current_quantity);

					ad.max_queue_size= atmik::config::get<std::size_t>(prefix + ".max_queue_size", f_data.max_queue_size);
				}
				return actual_values;
				
			}


		}

		

		atmik::simulation::fuel_storage::fuel_storage(atmik::simulation::fuel fuel_type) : fuel_type(fuel_type) 
		{
			static const fuel_map fuel_values = get_fuel_data();

			assert(fuel_values.find(fuel_type) != fuel_values.end());
			if (fuel_values.find(fuel_type) == fuel_values.end()) throw std::logic_error("bad fuel value");
			const auto& ad = fuel_values.find(fuel_type)->second;
			fuel_rate = ad.fuel_rate;
			capacity = ad.capacity;
			current_quantity = ad.current_quantity;

			// create vector of service bays with particular fuel type
			while ( bays.size() < ad.num_bays ) bays.push_back(service_bay(this));

			queue_v pending1(ad.max_queue_size);// create a empty queue
			pending = pending1; 
		}

	}
}