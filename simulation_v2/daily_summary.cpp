#include"daily_summary.h"
#include "fuel_storage.h"
#include "service_bay.h"
#include"fuel.h"
#include<iostream>
#include<fstream>
#include<cassert>
#include "win_utils.h"
#include"publisher.h"
#include "log_dir.h"


namespace atmik
{
	namespace simulation
	{
		namespace reports
		{
			namespace
			{
				const std::string daily_log_file_name = atmik::utils::log_dir() + 
					                                     "/daily_log.txt";

				std::ostream& daily_log_stm()
				{
					static std::ofstream file(daily_log_file_name);
					assert(file.is_open());
					return file;
				}

				struct daily_stats {
					unsigned int day_num = 0;
					unsigned int total_vehicles;
					double fuel_qty_sold;
					double money_made;
					unsigned int num_tank_refuels;
				};
				std::vector<daily_stats> days_stats; 
				daily_stats ds;
			}

			void daily_stats_listener(event e)
			{
				assert(e.type == event::END_SERVICE);
				if (e.type == event::event_type::END_SERVICE)
				{
					const auto& bay = service_bay::look_up(e.info);
					const auto& v = vehicle::look_up(bay.vehicle_being_serviced());
					++ds.total_vehicles;
					++ds.fuel_qty_sold += v.fuel_required;
					ds.money_made += v.fuel_required * price_per_litre(v.ftype);
				}
			}

			void daily_tank_refill_listener(event e)
			{
				assert(e.type == event::TANK_REFILL_END);
				if (e.type == event::TANK_REFILL_END)
					++ds.num_tank_refuels;
			}

			void re_initialize_new_day(event e)
			{
				assert(e.type == event::START_OF_DAY);
				if (e.type == event::START_OF_DAY)
				{
					++ds.day_num;
					ds.fuel_qty_sold = 0;
					ds.money_made = 0;
					ds.num_tank_refuels = 0;
					ds.total_vehicles = 0;
				}
			}

			void day_stats(event e)
			{
				assert(e.type == event::END_OF_DAY);
				if (e.type == event::END_OF_DAY)
				{
					daily_log_stm() << "\n-------------DAILY_REPORT- DAY " << ds.day_num << "------------\n"
						<< "\tTotal number of vehicles services: " << ds.total_vehicles
						<< "\n\tMoney made: " << ds.money_made
						<< "\n\tNumber of Tank Refuels: " << ds.num_tank_refuels
						<< "\n\tFuel Quantity Sold: " << ds.fuel_qty_sold <<
						"\n";
					days_stats.push_back(ds);
				}
			}
			void connect_daily_listeners()
			{
				auto& publisher = event_publisher::default_publisher();
				publisher.add_listener(event::TANK_REFILL_END, reports::daily_tank_refill_listener);
				publisher.add_listener(event::END_SERVICE, reports::daily_stats_listener);
				publisher.add_listener(event::END_OF_DAY, reports::day_stats);
				publisher.add_listener(event::START_OF_DAY, reports::re_initialize_new_day);
			}

			void print_stats_days(bool offline)
			{
				if (offline)
				{
					daily_log_stm() << std::flush;
					// assume it is windows (for now)
					atmik::utils::win::create_process( 
						atmik::utils::text_viewer_program_name() + ' '  + 
						daily_log_file_name );
				}
				else
				{
					daily_log_stm() << std::flush;
					for (auto ds : days_stats)
					{
						std::cout << "\n-------------DAILY_REPORT- DAY " << ds.day_num << "------------\n"
							<< "\tTotal number of vehicles services: " << ds.total_vehicles
							<< "\n\tMoney made: " << ds.money_made
							<< "\n\tNumber of Tank Refuels: " << ds.num_tank_refuels
							<< "\n\tFuel Quantity Sold: " << ds.fuel_qty_sold <<
							"\n";
					}
				}
			}
		}
	}
}