#define _CRT_SECURE_NO_WARNINGS
#include "win_utils.h"
#include <windows.h>
#include <cstring>
#include <iostream>

namespace atmik
{
	namespace utils
	{
		namespace win
		{
			bool create_process(const std::string& cmd_line, bool wait )
			{
				STARTUPINFOA si{};
				si.cb = sizeof(si);
				si.wShowWindow = SW_SHOWDEFAULT;
				PROCESS_INFORMATION pi{};
				constexpr std::size_t CMD_LINE_BUF_SZ = 1024;
				if (cmd_line.size() >= CMD_LINE_BUF_SZ) return false;
				char cmd_line_cstr[CMD_LINE_BUF_SZ];
				std::strcpy(cmd_line_cstr, cmd_line.c_str());
				BOOL result = ::CreateProcessA(nullptr, cmd_line_cstr, nullptr, nullptr,
					false, 0, nullptr, nullptr, std::addressof(si), std::addressof(pi));
				if (result == FALSE)
				{
					const auto ec = ::GetLastError();
                    #ifndef NDEBUG
					std::cerr << "CreateProcess failed, error == " << ec << '\n';
                    #endif // NDEBUG
					return false;
				}
				::CloseHandle(pi.hThread);

				if (wait) ::WaitForSingleObject(pi.hProcess, INFINITE);
				::CloseHandle(pi.hProcess);
				return true;
			}
		}
	}
}