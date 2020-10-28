#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include<queue>
#include <stdexcept>

namespace atmik
{
	namespace simulation
	{
		struct event
		{
			enum event_type { START_SIMULATION, PETROL_ARRIVAL,DIESEL_ARRIVAL,LPG_ARRIVAL, END_PETROL
				,END_DIESEL,END_LPG, END_SIMULATION, GENERATE_REPORT };

			event(event_type e, unsigned int time, int info) : e(e), time(time), info(info), id(++next_id) {}

			event_type e; 
			int info = 0; 
			unsigned int time; 
			unsigned int id;

			static unsigned int next_id;

			// binary predicate to order events based on priority
			// priority is basd on time; earlier events have higher priority
			bool operator< (const event& that) const { return time > that.time;  }

			static std::priority_queue<event> queued_events;

			

			static void add_event( event_type e, unsigned int time, int info)
			{
				queued_events.push(event{ e,time,info });
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


		};

		inline bool empty() { return event::queued_events.empty(); }
	}
}

#endif // !EVENT_H_INCLUDED
