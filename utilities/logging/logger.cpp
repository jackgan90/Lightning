#include "logger.h"

namespace LightningGE
{
	namespace Utility
	{
		const char* LogFileName = "log.txt";
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
