#pragma once
#include <string>
#include <fstream>
#include "common.h"
#include "timesystem.h"

namespace LightningGE
{
	namespace Utility
	{
		const char* LogFileName = "log.txt";
		enum LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		class DLLEXPORT Logger
		{
		public:
			Logger();
			~Logger();
			template<typename... Args>
			void Log(LogLevel level, std::string text, Args... args)
			{
				auto prefix = LogLevelToPrefix(level);
				auto timeStr = Foundation::Time::currentTimeString();
				char buf[256];
				sprintf(buf, text.c_str(), std::forward<Args>(args)...);
				m_fs << timeStr << " " << prefix << " " << buf << std::endl;
			}
		private:
			std::string LogLevelToPrefix(LogLevel level)const;
			std::fstream m_fs;
		};
		Logger logger;
	}
}
