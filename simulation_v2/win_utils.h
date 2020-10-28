#ifndef WIN_UTILS_H_INCLUDED
#define WIN_UTILS_H_INCLUDED

#include <string>

namespace atmik
{
	namespace utils
	{
		namespace win
		{
			bool create_process(const std::string& cmd_line, bool wait = false);
		}
	}
}


#endif // WIN_UTILS_H_INCLUDED
