#include "event_log.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include"win_utils.h"
#include"log_dir.h"

namespace atmik
{
	namespace simulation
	{
		namespace reports
		{
			namespace
			{
				const std::string event_log_file_name = atmik::utils::log_dir() + "/event_log.txt";

				std::ostream& event_log_stm()
				{
					static std::ofstream file(event_log_file_name);
					return file;
				}
			}

			void all_events_listener(event e)
			{
				event_log_stm() << e << '\n';
			}

			void print_event_log(bool offline )
			{
				if (offline)
				{
					event_log_stm() << std::flush;
                    // assume it is windows (for now)
					static const auto viewer = atmik::utils::text_viewer_program_name();
					atmik::utils::win::create_process( viewer + ' ' + event_log_file_name);
				}

				else
				{
					event_log_stm() << std::flush;
					std::cout << "\n\nEVENT LOG\n---------\n"
						<< std::ifstream(event_log_file_name).rdbuf()
						<< "\n-----------------------------------\n";
				}
			}

		}
	}
}

