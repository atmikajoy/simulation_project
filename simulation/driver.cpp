#include "driver.h"
#include<vector>
#include<map> 
#include "../tee/report_writer.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace atmik
{
	namespace simulation
	{
		std::vector<vehicle> vehicles; 
		std::map<unsigned int, vehicle> vehicle_map; 
		clock driver::clk({ 8,0,0 }); 
		bool end_simul_ = false; 
		fuel_storage petrol_tank(fuel::PETROL);
		fuel_storage diesel_tank(fuel::DIESEL);
		fuel_storage lpg_tank(fuel::LPG);
		std::vector<std::string> petrol_vehicles; 
		std::vector<std::string> diesel_vehicles;
		std::vector<std::string> lpg_vehicles;

		struct stats
		{
			unsigned long long num_serviced = 0;
			unsigned long long num_unserviced = 0;
			unsigned long long total_service_time = 0 ;
			unsigned long long total_wait_time = 0;
			unsigned int max_wait_time = 0;

			// compute:
			// %serviced
			// average_wait_time
			// average_service_time
		};

		std::map<fuel, stats> stats_map{ { fuel::PETROL, {} }, { fuel::DIESEL, {} }, { fuel::LPG, {} } };

		void schedule_next_vehicle( const vehicle& v_arrived )
		{
			if (end_simul_ == true)
			{
				std::cout << "No more simulations";
				return;
			}

			auto v = v_arrived.next_vehicle();
			vehicle_map[v.id] = v;
			switch (v.fuel_type)
			{
			case fuel::PETROL: event::add_event(event::PETROL_ARRIVAL, v.arrival_time, v.id); break;
			case fuel::DIESEL: event::add_event(event::DIESEL_ARRIVAL, v.arrival_time, v.id); break;
			case fuel::LPG: event::add_event(event::LPG_ARRIVAL, v.arrival_time, v.id); break;
			
			}
			

		}

		void start_simulation()
		{
			for (vehicle::v_type type : { vehicle::CAR, vehicle::HEAVY, vehicle::TWO_WHEEL, vehicle::THREE_WHEEL })
			{
				fuel f = fuel::PETROL;
				vehicle next = vehicle(type, f ).next_vehicle();
				vehicle_map[next.id] = next;
				switch (next.fuel_type)
				{
					case fuel::PETROL: event::add_event(event::PETROL_ARRIVAL, next.arrival_time, next.id); break;
					case fuel::DIESEL: event::add_event(event::DIESEL_ARRIVAL, next.arrival_time, next.id); break;
					case fuel::LPG: event::add_event(event::LPG_ARRIVAL, next.arrival_time, next.id); break;

				}
			}
			
		}

		std::size_t select_bay( const fuel_storage& fuel_t)
		{

			for (std::size_t i = 0; i < fuel_t.bays.size(); ++i) 
				if (!fuel_t.bays[i].occupied_())
				{
					return i;
				}
					
				
					

			//const auto iter = std::min_element(fuel_storage::bays.begin(), fuel_storage::bays.end(),
				//[](const auto& a, const auto& b) { return a..size() < b.pending.size(); });

			return fuel_t.bays.size();
		}
		bool check_current_quantity(fuel_storage& fs_temp, double fuel_min)
		{
			return fs_temp.current_quantity - fuel_min >= 0;
		}
		int process_vehicle_arrival(unsigned int v_id)
		{
			vehicle v_temp = vehicle_map[v_id];
			vehicle::last_arrival_time[v_temp.type] = v_temp.arrival_time;
			//v_temp.arrival_time = driver::clk();
			std::cout << "\n A" << v_temp.to_string() << "-> id" << v_temp.id 
				<< "-> arrived at" << driver::clk.to_string(v_temp.arrival_time) << '\n';
			schedule_next_vehicle(v_temp);

			std::size_t bay_id = 0; 

			fuel_storage* f_temp = &petrol_tank; 
			
			event::event_type e_type = event::END_PETROL;

			if (v_temp.fuel_type == fuel::PETROL);
			else if (v_temp.fuel_type == fuel::DIESEL)
			{
				f_temp = &diesel_tank;
				e_type = event::END_DIESEL;
			}
			else
			{
				f_temp = &lpg_tank;
				e_type = event::END_LPG;
			}
			bay_id = select_bay(*f_temp);
			if (bay_id == f_temp->bays.size())
			{
				if (!f_temp->pending.enqueue(v_temp))
				{
					std::cout << "\n------------------------------------------\n";
					std::cout << "vehicle #" << v_temp.id << " queue full (" << v_temp.to_string() 
						      << ")\n";

					++stats_map[v_temp.fuel_type].num_unserviced;
				}
				else std::cout << " queued\n";
				return bay_id;
			}
			
			service_bay& b = (*f_temp).bays[bay_id];


			if (!b.occupied_())
			{
				unsigned int refilling_time = 0;

				if (check_current_quantity(*f_temp, v_temp.fuel_required_litres()))
					(*f_temp).deduct_fuel(v_temp.fuel_required_litres());
				else
				{
					std::cout << "Please wait for" << to_string((*f_temp).fuel_type) << "tank to fill\n";
					(*f_temp).current_quantity = (*f_temp).capacity;
					refilling_time = (*f_temp).refilling_time();
					std::cout << to_string((*f_temp).fuel_type) << " tank filled\n";
				}

				auto serv_start = driver::clk() + refilling_time;
				b.start_service(serv_start, v_temp, bay_id);
				event::add_event(e_type, b.end_service_time(), bay_id);
				std::cout << "\tserviced in free bay #" << b.descr()
					<< "  start: " << driver::clk.to_string(serv_start)
					<< "  end: " << driver::clk.to_string(b.end_service_time()) << '\n';
			}
		
			else if ((*f_temp).pending.enqueue(v_temp))
			{
				std::cout << "\tsent to free/shorter queue at bay #" << bay_id
					<< "  queue size after enque " << (*f_temp).pending.size() << '\n';
			}
			else
			{
				std::cout << "\toccupied, queues are full, unserviced\n";
				vehicles.push_back(v_temp);
			}

			return bay_id;

		}

		void end_vehicle_service(event::event_type e,fuel_storage& fs, int bay)
		{
			auto& b = fs.bays[bay];
			const vehicle& v = b.last_serviced;
			std::cout << "\n ------------- \n A" << v.to_string() << "-> id" << v.id
				<< "-> end service in bay " << b.descr() 
				<< " at " << driver::clk.to_string() << '\n';

			auto& stats = stats_map[v.fuel_type];

			++stats.num_serviced;
			stats.total_service_time += (v.end_service - v.start_service);
			auto wait_time = v.wait_time();
			stats.total_wait_time += wait_time;
			stats.max_wait_time = std::max(stats.max_wait_time, wait_time );

			vehicles.push_back(v);
			fs.bays[bay].end_service();
			if (!fs.bays[bay].storage->pending.empty())
			{
				vehicle v1 = fs.bays[bay].storage->pending.dequeue();
				fs.bays[bay].start_service(driver::clk(), v1, bay);
				event::add_event(e, fs.bays[bay].end_service_time(), bay);
			}
			std::stringstream stm;
			stm << std::setw(4) << v.id << std::setw(12)
				<< b.descr() << std::setw(10)
				<< std::to_string(v.start_service) << std::setw(10)
				<< std::to_string(v.end_service) << std::setw(8)
				<< std::to_string(v.end_service - v.start_service) << std::setw(12)
				<< std::to_string(v.wait_time()) << std::setw(9);

			auto str_details = stm.str();
			if (v.fuel_type == fuel::PETROL) petrol_vehicles.push_back(str_details);
			else if (v.fuel_type == fuel::DIESEL) diesel_vehicles.push_back(str_details);
			else if (v.fuel_type == fuel::LPG) lpg_vehicles.push_back(str_details);
	
		}

		namespace
		{
			std::string to_mmm_ss(unsigned long long secs)
			{
				std::ostringstream stm;
				stm << secs / 60 << ':' << std::setw(2) << std::setfill('0') << secs % 60;
				return stm.str();
			}

			std::string to_mmm_ss(unsigned int secs) 
			{ 
				return to_mmm_ss((unsigned long long) secs);
			}

			std::string to_mmm_ss(double secs)
			{ 
				unsigned long long s = (unsigned long long)secs;
				unsigned int msecs = (unsigned int) ( ( secs - s ) * 1000 ) ;
				std::ostringstream stm;
				stm << s / 60 << ':' << std::setw(2) << std::setfill('0') << s % 60
					<< '.' << std::fixed << std::setw(3) << msecs;
				return stm.str();
			}
		}

		void gen_report()
		{
			atmik::util::report_writer::write({ "v_id Bay_Number Start_Time End_time Service_time Wait_time" });
			for (auto f_type : { petrol_vehicles, diesel_vehicles, lpg_vehicles })
				atmik::util::report_writer::sync_write(f_type);
		
			atmik::util::report_writer::sync_write({ "Vehicles that couldnt be serviced are:" });
			for (auto nt_serv : vehicles)
			{
				atmik::util::report_writer::sync_write({ std::to_string(nt_serv.id) + " " + nt_serv.to_string() });
			}

			std::vector<std::string> stats_report;
			// const auto to_mm_ss = [] ( )

			for (auto fuel_t : { fuel::PETROL, fuel::DIESEL,fuel::LPG })
			{
				const auto& s = stats_map[fuel_t];
				std::ostringstream stm;
				stm << std::fixed << std::setprecision(2);
				stm << "\nsummary for fiel type " << to_string(fuel_t) << "\n--------------\n"
					<< "avg_serv_time " << std::setw(12) 
					<< to_mmm_ss( double(s.total_service_time) / s.num_serviced ) << '\n'
					<< "avg_wait_time " << std::setw(12) 
					<< to_mmm_ss( double(s.total_wait_time) / s.num_serviced ) << '\n'
					<< "max_wait_time " << std::setw(12) << to_mmm_ss(s.max_wait_time) << '\n'
					<< " percent_serv " << std::setw(12) 
					<< (s.num_serviced * 100.0) / (s.num_serviced + s.num_unserviced) << "%\n";

				stats_report.push_back(stm.str());
			}

			atmik::util::report_writer::write(std::move(stats_report));

		}

		void driver::run()
		{

			while (!empty())
			{
				auto ev = event::next_event();
				driver::clk = ev.time;
				switch (ev.e)
				{
				case event::START_SIMULATION:
					start_simulation();
					break;
				case event::PETROL_ARRIVAL:
				case event::DIESEL_ARRIVAL:
				case event::LPG_ARRIVAL:
					process_vehicle_arrival(ev.info);
					break;
				case event::END_PETROL:
					end_vehicle_service(ev.e, petrol_tank, ev.info);
					break;
				case event::END_DIESEL:
					end_vehicle_service(ev.e, diesel_tank, ev.info);
					break;
				case event::END_LPG: 
					end_vehicle_service(ev.e, lpg_tank, ev.info); 
					break;
				case event::END_SIMULATION: 
					end_simul_ = true;
					event::add_event(event::GENERATE_REPORT, ev.time + 100000, 0);
					break;
				case event::GENERATE_REPORT:
					gen_report();
					break;

				}

			}

		}
	}
}