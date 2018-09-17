#include "logger.h"

namespace Lightning
{
	namespace Foundation
	{
		const char* LogFileName = "log.txt";
		LIGHTNING_FOUNDATION_API Logger logger;
		Logger::Logger()
		{
			mFs.open(LogFileName, std::fstream::out);
		}

		Logger::~Logger()
		{
			mFs.flush();
			mFs.close();
		}

		std::string Logger::LogLevelToPrefix(LogLevel level)const
		{
			switch (level)
			{
			case LogLevel::Debug:
				return "[Debug]";
			case LogLevel::Info:
				return "[Info]";
			case LogLevel::Warning:
				return "[Warning]";
			case LogLevel::Error:
				return "[Error]";
			default:
				return "[Unknown]";
			}
			return "";
		}
	}
}
