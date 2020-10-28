#ifndef DAILY_SUMMARY_H_INCLUDED
#define DAILY_SUMMARY_H_INCLUDED

#include "event.h"
#include<vector>

namespace atmik
{
	namespace simulation
	{
		namespace reports
		{
			void daily_stats_listener(event e);

			void tank_refill_listener(event e);

			void re_initialize_new_day(event e);

			void day_stats(event e);

			void connect_daily_listeners();

			void print_stats_days(bool offline);
		}
	}
}




#endif // !DAILY_SUMMARY_H_INCLUDED
