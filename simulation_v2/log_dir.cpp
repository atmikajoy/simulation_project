#define _CRT_SECURE_NO_WARNINGS
#include "log_dir.h"
#include <ctime>
#include <filesystem>

namespace atmik
{
	namespace utils
	{
		namespace
		{
			std::string log_dir_stem()
			{
				static const std::string stem = "C:/simulation/logs_";

				return stem;
			}

			std::string time_stamp()
			{
				const auto t = std::time(nullptr);
				char buffer[1024];
				std::strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", std::localtime(std::addressof(t)));
				return buffer;
			}
		}

		std::string log_dir()
		{
			static const std::string dir_path = log_dir_stem() + time_stamp();

			namespace fs = std::filesystem;
			if (!fs::exists(dir_path)) fs::create_directory(dir_path);
			if (!fs::is_directory(dir_path)) 
				throw std::runtime_error("file exisys with output directory name");

			return dir_path;
		}

		std::string text_viewer_program_name()
		{
			return "notepad.exe";
		}
	}
}
