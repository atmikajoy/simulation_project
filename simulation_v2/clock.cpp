#include "clock.h"
#include <sstream>
#include <iomanip>
#include <cassert>
#include <iostream>
#include <limits>


// unsigned int atmik::simulation::clock::day = 1;

namespace atmik
{
	namespace simulation
	{
		
		namespace
		{
			
			//static_assert( max_seconds < std::numeric_limits<unsigned int>::max() );

			bool valid(const clock::time_of_day_t& tod)
			{
				return tod.hr >= 0 && tod.hr < 24 &&
					tod.min >= 0 && tod.min < 60 &&
					tod.sec >= 0 && tod.sec < 60;
			}

			unsigned int to_seconds(const clock::time_of_day_t& tod, int day_num = 0 )
			{
				assert(valid(tod));
				return tod.hr * 60 * 60 + tod.min * 60 + tod.sec + day_num * clock::seconds_per_day ;
			}

			clock::time_of_day_t from_seconds(unsigned int secs)
			{
			    secs %= clock::seconds_per_day;
				int s = secs % 60;
				secs /= 60;
				int m = secs % 60;
				secs /= 60;
				clock::time_of_day_t tod{ int(secs) % 24, m, s };
				assert(valid(tod));
				return tod;
			}

		}

		clock::clock(time_of_day_t t, int day_num) : seconds(to_seconds(t, day_num)) {}

		std::string clock::to_string(unsigned int elapsed_secs) const
		{
			const int nday = elapsed_secs / seconds_per_day;
			elapsed_secs %= seconds_per_day;
			const auto t = from_seconds(elapsed_secs);
			std::ostringstream stm;
			stm << 'D' << std::setw(3) << std::setfill('0') << nday+1 << '.'
				<< std::setw(2) << t.hr << ':'
				<< std::setw(2) << t.min << ':' << std::setw(2) << t.sec;

			return stm.str();
		}

		clock& clock::operator= (unsigned int secs)
		{
			seconds = secs;
			{
			}
			return *this;
		}

		clock& clock::operator+= (unsigned int secs)
		{
			seconds += secs;
			return *this;
		}

		unsigned int clock::operator() () const
		{
			return seconds;
		}

		clock::time_of_day_t clock::time_of_day() const
		{
			return from_seconds(seconds);

		}

		std::string clock::to_string() const
		{
			return to_string(seconds);
		}

		std::ostream& operator<< (std::ostream& stm, const atmik::simulation::clock& clk)
		{
			return stm << clk.to_string() << " (" << clk() << ')';
		}

		clock& default_clock()
		{
			static clock clk;
			return clk;
		}
	}
}