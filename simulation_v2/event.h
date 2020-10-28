#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <queue>
#include <stdexcept>
#include <iosfwd>
#include <vector>

namespace atmik
{
	namespace simulation
	{
		struct event
		{
			enum event_type 
			{
				 START_SIMULATION /* suggested priority == 99 */, 
				 VEHICLE_ARRIVAL, /* suggested priority == 10, info == vehicle id */
				 START_SERVICE, /* suggested priority == 20, info == bay id */
				 END_SERVICE, /* suggested priority == 21, info == bay id */
				 START_OF_DAY, /* suggested priority == 5, freah arrivals start */
				 END_OF_DAY, /* suggested priority == 5, no more arrivals are allowed */
				 END_SIMULATION, /* suggested priority == 1, finished program*/
				 GENERATE_REPORT, /* suggested priority == 0, info == report id, default == all*/
				 TANK_REFILL_START,/*suggested priority == 6,service existing vehicles, info == fuel_type */
				 TANK_REFILL_END/* suggested priority == 7, info == fuel_type*/
				 // add other events as required
			};

			static const std::vector<event_type>& event_type_list();

			event( event_type type, unsigned int time,
				   unsigned int priority = 0, unsigned int info = 0 ) 
				: type(type), time(time), priority(priority), info(info), id(++next_id) {}

			// events are maintained in a priority queue
			// priority is based on time of event (earlier events have higher priority)
			// for two events with the same time, the higher priority event takes precedence
			event_type type;
			unsigned int info = 0; // associated object's unique id
			unsigned int time; // time of occurrence (elapsed seconds) 
			unsigned int priority; // 0 == lowest priority
			unsigned int id; // ubique id of the event (used for testing, tracking)
			bool handled = false;

			static unsigned int next_id;

			// binary predicate to order events based on time, priority
			// priority is basd on time; earlier events have higher priority
			bool operator< (const event& that) const 
			{ 
				if (time == that.time) return priority < that.priority;
				return time > that.time; 
			}

			static std::priority_queue<event> queued_events;

			static void add_event( event_type type, unsigned int time, 
				                   unsigned int priority = 0, unsigned int info = 0 )
			{
				queued_events.push( event{ type, time, priority, info });
			}

			static event next_event()
			{
				if (!queued_events.empty())
				{
					auto e = queued_events.top();
					queued_events.pop();
					return e;
				}

				else throw std::out_of_range("empty queue: no pending events");


			}

			static inline bool empty() { return queued_events.empty(); }
		};

		std::ostream& operator<< (std::ostream&, const event&);

	}
}

#endif // !EVENT_H_INCLUDED
