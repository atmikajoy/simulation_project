#include "report_writer.h"
#include "log.h"
#include "thread_pool.h"
#include <mutex>

namespace atmik
{
	namespace util
	{
		namespace
		{
			void do_write(std::vector<std::string> lines_to_write)
			{
				static auto& logstm = log();
				static std::mutex log_mutex;

				std::lock_guard lock(log_mutex);
#ifndef NDEBUG
				//logstm << "*** INFO *** report written by thread " << std::this_thread::get_id()
				//	<< "\n----------------------\n";
#endif // NDEBUG
				for (const auto& str : lines_to_write) logstm << str << '\n';
				logstm << std::flush;
			}
		}
		namespace report_writer
		{

			void write( std::vector<std::string> lines)
			{
				thread_pool::default_pool().queue_task( do_write, std::move(lines) );
			}

			void sync_write(std::vector<std::string> lines)
			{
				do_write( std::move(lines));
			}
		}
	}
}
