#ifndef SERVICE_BAY_H_INCLUDED
#define SERVICE_BAY_H_INCLUDED

#include "vehicle.h"
#include "queue.h"
#include "fuel.h"

namespace atmik
{
	namespace simulation
	{
		struct fuel_storage;

		struct service_bay
		{
			service_bay(fuel_storage* storage);
				
			void start_service(unsigned int start_time, vehicle v, int bay);

			unsigned int end_service_time() const ;

			void end_service();

			bool occupied_() const { return occupied; }

			std::string descr() const;

			vehicle last_serviced;
			fuel_storage* storage;
		private:
			bool occupied;
			
			 
			static int next_bay_id;

			unsigned int service_end_at=-1;
			int id = ++next_bay_id;
		};
	}
}

#endif //SERVICE_BAY_H_INCLUDED