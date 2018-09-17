#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "timesystem.h"

namespace Lightning
{
	namespace Foundation
	{
		std::string Time::GetCurrentTimeString()
		{
			auto now = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(now);
			std::stringstream ss;
			ss << std::put_time(std::localtime(&tt), "[%Y-%m-%d %H:%M:%S]");
			return ss.str();
		}
	}
}