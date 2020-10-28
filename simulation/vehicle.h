#ifndef VEHICLE_H_INCLUDED
#define VEHICLE_H_INCLUDED

#include"fuel.h"
#include<map>
namespace atmik
{
	namespace simulation
	{
		struct vehicle
		{
			enum v_type { CAR, HEAVY,TWO_WHEEL, THREE_WHEEL};
			static constexpr std::size_t NTYPES = 4;
			static_assert(NTYPES == THREE_WHEEL + 1);

			fuel fuel_type; 
			v_type type;

			vehicle(v_type t, fuel f) : fuel_type(f), type(t)
			{
				fuel_reqd = compute_fuel_required_litres();
			}
			
			vehicle() : id(-1) { }
			struct dummy_id {};

			// vehicle(unsigned int arrival_time) : arrival_time(arrival_time) { last_arrival_time = arrival_time; };

			friend std::ostream& operator << (std::ostream& stm, const vehicle& v);

			unsigned int wait_time() const { return start_service - arrival_time; }
			unsigned int service_time() const { return end_service - extra_service_time_seconds() - start_service; };
			

			std::string to_string() const;

			std::string type_string() const;

			vehicle next_vehicle() const;

			double fuel_required_litres() const { return fuel_reqd; }

			int extra_service_time_seconds() const;

			unsigned int arrival_time = 0;
			unsigned int start_service = -1;
			unsigned int end_service = -1;


			int id = ++vehicle_id;
			static unsigned int last_arrival_time[NTYPES];
			static int vehicle_id;

			vehicle(v_type t, fuel f, unsigned int arrival_time, dummy_id) 
				: arrival_time(arrival_time), id(-1), fuel_type(f), type(t) {
				fuel_reqd = compute_fuel_required_litres();
			}
				
			vehicle(v_type t, fuel f, unsigned int arrival_time )
				: arrival_time(arrival_time), fuel_type(f), type(t) {
				last_arrival_time[t] = arrival_time;
				fuel_reqd = compute_fuel_required_litres();
			}

		    private:
				double fuel_reqd ;
				double compute_fuel_required_litres() const;
				
		};

		
	}
}


#endif VEHICLE_H_INCLUDED
