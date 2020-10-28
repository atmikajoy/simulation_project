#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include <string>
namespace atmik
{


	namespace simulation
	{
		struct clock
		{
			struct time_of_day_t { int hr; int min; int sec; };

			clock() = default;
			clock(time_of_day_t t) : start_time(t) {}

			clock(unsigned int sec) : seconds(sec) {}; 

			unsigned int operator() () const;
			clock& operator= (unsigned int secs);

			time_of_day_t time_of_day() const;
			std::string to_string() const;

			std::string to_string(unsigned int elapsed_secs) const;

		private:
			unsigned int seconds = 0;
			time_of_day_t start_time{};
		};
	}
}

#endif // !CLOCK_H_INCLUDED
