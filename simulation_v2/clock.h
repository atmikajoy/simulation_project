#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include <string>
#include <iosfwd>
#include <limits>

namespace atmik
{
	namespace simulation
	{
		// virtual clock; resolution is 1 second
		struct clock
		{
			static constexpr unsigned int seconds_per_day = 24 * 60 * 60;
			static constexpr unsigned int max_days
				= std::numeric_limits<unsigned int>::max() / seconds_per_day;

			struct time_of_day_t { int hr; int min; int sec; };

			clock() = default;
			clock(time_of_day_t t, int day_num = 0);

			clock(unsigned int sec) : seconds(sec) {};

			// return the current virtual time (elapsed seconds)
			unsigned int operator() () const;

			//update the current virtual time (elapsed seconds)
			clock& operator= (unsigned int secs);

			// increment the current virtual time (by seconds)
			clock& operator+= (unsigned int secs);

			time_of_day_t time_of_day() const;
			std::string to_string() const;

			std::string to_string(unsigned int elapsed_secs) const;
			// unsigned int day;
		private:
			unsigned int seconds = 0; // virtual clock always starts at zero elapsed seconds
			// time_of_day_t start_time{}; // actual time of day when virtual clock started

			
		};

		std::ostream& operator<< (std::ostream&, const clock&);

		inline clock operator+ (clock clk, unsigned int secs) { return clk += secs;  }

		// the default clock used by all the modules in the simulation
		// it is the responsiblity of the initialisation code 
		//    to set the start_time of this clock 

		clock& default_clock();
	}


}



#endif // !CLOCK_H_INCLUDED
