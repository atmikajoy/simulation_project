#ifndef FUEL_H_INCLUDED
#define FUEL_H_INCLUDED

#include <string>

namespace atmik
{
	namespace simulation
	{
		enum class fuel { PETROL, DIESEL, LPG };

		inline std::string to_string(fuel f)
		{
			switch (f)
			{
			case fuel::PETROL: return "petrol";
			case fuel::DIESEL: return "diesel";
			case fuel::LPG: return "LPG";
			}

			return "";
		}

		double price_per_litre(fuel f); 
	}
}

#endif // FUEL_H_INCLUDED