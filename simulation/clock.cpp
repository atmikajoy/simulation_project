#include "clock.h"
#include <sstream>
#include <iomanip>

namespace atmik
{
	namespace simulation
	{
		namespace
		{
			unsigned int to_seconds(clock::time_of_day_t tod)
			{
				return tod.hr * 60 * 60 + tod.min * 60 + tod.sec;
			}

			clock::time_of_day_t from_seconds(unsigned int secs)
			{
				int s = secs % 60;
				secs /= 60;
				int m = secs % 60;
				secs /= 60;
				return { int(secs) % 24, m, s };
			}
		}

		std::string clock::to_string(unsigned int elapsed_secs) const
		{
			const auto t = from_seconds(elapsed_secs + to_seconds(start_time));
			std::ostringstream stm;
			stm << std::setfill('0') << std::setw(2) << t.hr << ':'
				<< std::setw(2) << t.min << ':' << std::setw(2) << t.sec;

			return stm.str();
		}

		clock& clock::operator= (unsigned int secs)
		{
			seconds = secs;
			return *this;
		}

		unsigned int clock::operator() () const
		{
			return seconds;
		}

		clock::time_of_day_t clock::time_of_day() const
		{
			return from_seconds(to_seconds(start_time) + seconds);

		}

		std::string clock::to_string() const
		{
			const clock::time_of_day_t temp = time_of_day();

			std::ostringstream stm;
			stm << std::setfill('0') << std::setw(2) << temp.hr << ':'
				<< std::setw(2) << temp.min << ':' << std::setw(2) << temp.sec;

			return stm.str();
		}
	}
}