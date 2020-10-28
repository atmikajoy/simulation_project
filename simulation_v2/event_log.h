#ifndef EVENT_LOG_H_INCLUDED
#define EVENT_LOG_H_INCLUDED

#include "event.h"

namespace atmik
{
	namespace simulation
	{
		namespace reports
		{

			void all_events_listener(event e);

			void print_event_log( bool offline = false );
		}
	}
}

#endif // EVENT_LOG_H_INCLUDED
