#include "fuel_summary.h"
#include <cassert>
#include "service_bay.h"
#include "vehicle.h"
#include <map>
#include <iomanip>
#include "fuel_storage.h"
#include"win_utils.h"
#include"log_dir.h"
namespace atmik
{
	namespace simulation
	{
		namespace reports
		{
			namespace
			{
				const std::string fuel_path = atmik::utils::log_dir() + "/fuel_summary.txt";
				
				std::ostream& fuel_log_stm()
				{
					static std::ofstream file(fuel_path);
					assert(file.is_open());
					return file;
				}
				
				struct fuel_stats
				{
					unsigned int num_vehicles = 0;
					double fuel_amount = 0;
					unsigned int num_tank_refills = 0;
				};

				std::map<fuel, fuel_stats> stats;
			}

			void overall_tank_refill_listener(event e)
			{
				assert(e.type == event::TANK_REFILL_END);
				if (e.type == event::TANK_REFILL_END)
				{
					++stats[fuel(e.info)].num_tank_refills;
				}
					
			}

			void fuel_summary_listener(event e)
			{
				assert(e.type == event::event_type::END_SERVICE);
				if (e.type == event::event_type::END_SERVICE)
				{
					const auto& bay = service_bay::look_up(e.info);
					const auto& v = vehicle::look_up( bay.vehicle_being_serviced() );
					++stats[v.ftype].num_vehicles;
					stats[v.ftype].fuel_amount += v.fuel_required;
				}
			}

			void printing_fuel_summary(std::ostream& stm)
			{
				stm << "\n----------\nfuel_summary report\n----------\n";
				for (const auto& [f, s] : stats)
				{
					const auto& storage = fuel_storage::look_up_fuel(f);

					stm << to_string(f) << "\n\tnum_vehicles: " << s.num_vehicles
						<< "\n\tfuel_amount: " << s.fuel_amount
						<< "\n\tprice per litre: " << price_per_litre(f)
						<< std::fixed << std::setprecision(2)
						<< "\n\ttotal price: " << s.fuel_amount * price_per_litre(f)
						<< "\n\tamount_remaining_in_tank: " << storage.fuel_remaining_in_tank() 
						<< "\n\tnumber of tank refills: "<<s.num_tank_refills
						<<'\n';
				}
				stm << "\n-------------end of fuel_summary report-------------\n" << std::flush;
			}

			void print_fuel_summary()
			{
				printing_fuel_summary(fuel_log_stm());
				auto report_veiwer_medium = atmik::utils::text_viewer_program_name();
				atmik::utils::win::create_process(report_veiwer_medium+ ' ' + fuel_path);
				printing_fuel_summary(std::cout);
			}




		}
	}
}