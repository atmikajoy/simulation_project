#ifndef REPORT_WRITER_H_INCLUDED
#define REPORT_WRITER_H_INCLUDED

#include <vector>
#include <string>

namespace atmik
{
	namespace util
	{
		namespace report_writer
		{
			void write( std::vector<std::string> lines );
			void sync_write(std::vector<std::string> lines);
		}
	}
}

#endif // REPORT_WRITER_H_INCLUDED
