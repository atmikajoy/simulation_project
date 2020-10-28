#ifndef VEHICLE_TYPE_H_INCLUDED
#define VEHICLE_TYPE_H_INCLUDED

#include <string>
#include "fuel.h"

namespace atmik
{
	namespace simulation
	{
		enum class vehicle_type { CAR, HEAVY_VEHICLE, TWO_WHEELER, THREE_WHEEELER };

		inline std::string to_string(vehicle_type v )
		{
			switch (v)
			{
				case vehicle_type::CAR: return "car";
				case vehicle_type::HEAVY_VEHICLE: return "heavy_vehicle";
				case vehicle_type::TWO_WHEELER: return "two_wheeler";
				case vehicle_type::THREE_WHEEELER: return "three_wheeler";
			}

			return "";
		}

		int arrival_rate(vehicle_type vt);  // per hour

		fuel select_fuel(vehicle_type vt);

		double fuel_required(vehicle_type vt);

		int extra_service_time(vehicle_type vt);


	}
}

#endif // VEHICLE_TYPE_H_INCLUDED