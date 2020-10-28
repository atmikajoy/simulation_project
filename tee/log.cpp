
#define _CRT_SECURE_NO_WARNINGS
#include "log.h"
#include <string>
#include <ctime>
#include "tee.h"
#include <fstream>
#include <filesystem>

namespace
{
	// TO DO: read this from config
	const std::string log_dir = "C:/simulation/logs";

	std::string log_file_name()
	{
		if (!std::filesystem::exists(log_dir))
			std::filesystem::create_directory(log_dir);

		const auto t = std::time(nullptr);

		// log_dir + "/" + YYYYMMDDHHMMSS" + '.log" ;
		 
		char buffer[1024];
		std::strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", std::localtime(std::addressof(t)));
		return log_dir + '/' + buffer + ".log";
	}
}

namespace atmik
{
	namespace util
	{
		namespace
		{
			struct helper_
			{
				helper_()
				{
					const auto fn = log_file_name();
					fbuf.open(fn, std::ios::out | std::ios::trunc);
				}

				std::filebuf fbuf;
			};
		}

		std::ostream& log()
		{
            /*
			static helper_ helper;
			static std::streambuf* pfilebuf = std::addressof(helper.fbuf);
			static teestream log_stm(std::clog.rdbuf(), pfilebuf );
			*/

			static std::ofstream file(log_file_name() );

			static teestream log_stm(file, std::clog);
			return log_stm;
		}
	}
}