#pragma once
#include <string>
#include <fstream>
#include "timesystem.h"
#include "utilitiesexportdef.h"

namespace LightningGE
{
	namespace Utility
	{
		enum LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		class LIGHTNINGGE_UTILITIES_API Logger
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
		LIGHTNINGGE_UTILITIES_API Logger logger;
	}
}
