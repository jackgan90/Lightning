#include "logger.h"
#include "timesystem.h"

using Time = LightningGE::Foundation::Time;
namespace LightningGE
{
	namespace Utility
	{

		using string = std::string;

		Logger::Logger()
		{
			m_fs.open(LogFileName, std::fstream::out);
		}

		Logger::~Logger()
		{
			m_fs.flush();
			m_fs.close();
		}

		void Logger::Log(LogLevel level, string text)
		{
			auto prefix = LogLevelToPrefix(level);
			auto timeStr = Time::currentTimeString();
			m_fs << timeStr << " " << prefix << " " << text;
		}

		string Logger::LogLevelToPrefix(LogLevel level)const
		{
			switch (level)
			{
			case Debug:
				return "[Debug]";
			case Info:
				return "[Info]";
			case Warning:
				return "[Warning]";
			case Error:
				return "[Error]";
			default:
				return "[Unknown]";
			}
			return "";
		}
	}
}
