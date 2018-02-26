#include "logger.h"

namespace LightningGE
{
	namespace Foundation
	{
		const char* LogFileName = "log.txt";
		LIGHTNINGGE_FOUNDATION_API Logger logger;
		Logger::Logger()
		{
			m_fs.open(LogFileName, std::fstream::out);
		}

		Logger::~Logger()
		{
			m_fs.flush();
			m_fs.close();
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
