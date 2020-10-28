#include "fuel.h"
#include <map>
#include "config.h"

namespace atmik
{
	namespace simulation
	{
		namespace
		{
			const std::string key_prefix = "simulation.fuel.";

			std::map< fuel, double > fuel_price_paise
			{ 
				{ fuel::PETROL, 8300 }, 
				{ fuel::DIESEL, 7780 },
				{ fuel::LPG, 3860 }
			};

			double get_fuel_price(fuel f)
			{
				const std::string key = key_prefix + to_string(f) + ".price_in_paise";
				return fuel_price_paise[f] = atmik::config::get<double>(key, fuel_price_paise[f]);
			}

			const std::map< fuel, double >& get_fuel_prices()
			{
				for (fuel f : { fuel::PETROL, fuel::DIESEL, fuel::LPG })
					get_fuel_price(f);
				return fuel_price_paise;
			}
		}

		double price_per_litre(fuel f)
		{
			static const auto& prices = get_fuel_prices();
			return fuel_price_paise[f] / 100.0 ;
		}
	}
}