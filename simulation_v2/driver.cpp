#include "driver.h"
#include "vehicle.h"
#include "service_bay.h"
#include "fuel_storage.h"
#include "clock.h"
#include <iosfwd>
#include "event.h"
#include "publisher.h"
#include <iostream>
#include"vehicle_summary.h"
#include "event_log.h"
#include "fuel_summary.h"
#include"daily_summary.h"
#include <chrono>

namespace atmik
{
	namespace simulation
	{
		namespace driver
		{
			void create_fuel_storage()
			{
				for (fuel f : { fuel::DIESEL, fuel::LPG, fuel::PETROL })
					fuel_storage::create(f);
			}

			void first_vehicles()
			{
				vehicle::schedulr_first_vehicles();
			}

			void connect_handlers()
			{
				vehicle::connect_events();
				service_bay::connect_events();
				for (auto& [id,fs] : fuel_storage::objects() ) fs.connect_events();
			}

			static void progress_bar_listener(event e)
			{
				using namespace std::chrono;
				static int n = 0;
				static const auto start = steady_clock::now();

				std::cout << '.' << std::flush;
				if (n % 50 == 49)
				{
					const auto curr = steady_clock::now();
					const auto elapsed = duration_cast<milliseconds>(curr - start).count();
					std::cout << ' ' << n + 1 << " days in " << elapsed/1000.0 
						      << " seconds\n" << std::flush;
				}
				++n;
			}

			void connect_listeners()
			{
				static auto& publisher = event_publisher::default_publisher();

				reports::connect_vehicle_summary_listeners();
				publisher.add_universal_listener(reports::all_events_listener);
				publisher.add_listener(event::END_SERVICE, reports::fuel_summary_listener);
				publisher.add_listener(event::TANK_REFILL_END, reports::overall_tank_refill_listener);
				reports::connect_daily_listeners();
				publisher.add_listener(event::END_OF_DAY, progress_bar_listener);
			}

			void init_days( clock::time_of_day_t start_time, unsigned int duration_secs_per_day,
				unsigned int ndays)
			{
				assert(ndays > 0 && ndays <= clock::max_days );

				unsigned int start_secs = start_time.hr * 60 * 60 +
					start_time.min * 60 + start_time.sec;
				unsigned int end_of_day_secs = start_secs + duration_secs_per_day;
				assert(end_of_day_secs < clock::seconds_per_day); // sanity check
				
				while (ndays > 0)
				{
					event::add_event(event::START_OF_DAY, start_secs);
					event::add_event(event::END_OF_DAY, end_of_day_secs);
					start_secs += clock::seconds_per_day;
					end_of_day_secs += clock::seconds_per_day; 
					--ndays;
				}
			}


			void init( clock::time_of_day_t start_time, unsigned int duration_secs_per_day,
				       unsigned int ndays)
			{
				init_days(start_time, duration_secs_per_day, ndays);
				create_fuel_storage();
				connect_handlers();
				connect_listeners();
				
			}

			void finish()
			{
				const auto wait = [] { std::cout << "press enter to continue: "; std::cin.get(); };

				reports::print_vehicle_arrival_summary();
				reports::print_fuel_summary();
				reports::print_stats_days(true);
				reports::print_event_log(true);
			}

			void run(clock::time_of_day_t start_time, unsigned int duration_secs_per_day, 
				     unsigned int ndays )
			{
				init(start_time, duration_secs_per_day, ndays );

				event_publisher& publisher = event_publisher::default_publisher();

				while (!event::empty())
				{
					event e = event::next_event();
					// std::cout << e << '\n';
					// std::cin.get();
					default_clock() = e.time;
					publisher.publish(e);
				}

				finish();
			}

			void run(clock::time_of_day_t start_time, clock::time_of_day_t end_time,
				unsigned int ndays)
			{
				const auto to_secs = [](clock::time_of_day_t tod)
				{ return tod.hr * 60 * 60 + tod.min * 60 + tod.sec; };

				const auto start = to_secs(start_time);
				const auto end = to_secs(end_time);
				assert(end > start);

				run(start_time, end - start, ndays);
			}

		}
	}
}