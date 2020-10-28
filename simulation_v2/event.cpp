#include "event.h"
#include <iostream>
#include <string>
#include <map>
#include"clock.h"

std::priority_queue<atmik::simulation::event> atmik::simulation::event::queued_events;

unsigned int atmik::simulation::event::next_id = 0;

namespace
{
	const std::map< atmik::simulation::event::event_type, std::string > event_strings
	{
		{ atmik::simulation::event::START_SIMULATION, "START_SIMULATION" },
		{atmik::simulation::event::VEHICLE_ARRIVAL, "VEHICLE_ARRIVAL" },
		{atmik::simulation::event::START_SERVICE, "START_SERVICE"},
		{atmik::simulation::event::END_SERVICE, "END_SERVICE"},
		{atmik::simulation::event::START_OF_DAY, "START_OF_DAY"},
		{atmik::simulation::event::END_OF_DAY, "END_OF_DAY"},
		{atmik::simulation::event::END_SIMULATION,"END_SIMULATION"},
		{atmik::simulation::event::GENERATE_REPORT,"GENERATE_REPORT"},
		{atmik::simulation::event::TANK_REFILL_START,"TANK_REFILL_START"},
		{atmik::simulation::event::TANK_REFILL_END,"TANK_REFILL_END"},
	};

	std::string to_string(atmik::simulation::event::event_type etype)
	{
		auto iter = event_strings.find(etype);
		if (iter != event_strings.end()) return iter->second;

		return "event type #" + std::to_string(etype);
	}
}

std::ostream& atmik::simulation::operator<< (std::ostream& stm, const atmik::simulation::event& ev )
{
	stm << ev.id << ' ' << ev.time << " ("
		<< atmik::simulation::default_clock().to_string(ev.time)
		<< ") " << to_string(ev.type);
	if (ev.info) stm << "  (object id == " << ev.info << ')';
	if (!ev.handled) stm << "  (unhandled)" ;
	return stm;
}

const std::vector<atmik::simulation::event::event_type>& 
atmik::simulation::event::event_type_list()
{
	static const std::vector<atmik::simulation::event::event_type> list =
	{
		START_SIMULATION,
		VEHICLE_ARRIVAL,
		START_SERVICE,
		END_SERVICE,
		START_OF_DAY,
		END_OF_DAY, 
		END_SIMULATION, 
		GENERATE_REPORT,
		TANK_REFILL_START,
		TANK_REFILL_END
    };

	return list;
}