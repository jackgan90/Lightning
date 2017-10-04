#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "timesystem.h"

using string = std::string;
namespace LightningGE
{
	namespace Foundation
	{
		string Time::currentTimeString()
		{
			auto now = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(now);
			std::stringstream ss;
			ss << std::put_time(std::localtime(&tt), "[%Y-%m-%d %H:%M:%S]");
			return ss.str();
		}
	}
}