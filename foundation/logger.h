#pragma once
#include <string>
#include <fstream>
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include "timesystem.h"
#include "foundationexportdef.h"

namespace LightningGE
{
	namespace Foundation
	{
		enum LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		class LIGHTNINGGE_FOUNDATION_API Logger
		{
		public:
			Logger();
			~Logger();
			template<typename... Args>
			void Log(LogLevel level, const std::string& text, Args... args)
			{
				//output to file
				auto prefix = LogLevelToPrefix(level);
				auto timeStr = Foundation::Time::currentTimeString();
				char buf[512];
				sprintf(buf, text.c_str(), std::forward<Args>(args)...);
				m_fs << timeStr << " " << prefix << " " << buf << std::endl;
#ifdef _MSC_VER
				char outputBuffer[512];
				sprintf_s(outputBuffer, "%s %s %s\n", timeStr.c_str(), prefix.c_str(), buf);
				::OutputDebugString(outputBuffer);
#endif
			}
		private:
			std::string LogLevelToPrefix(LogLevel level)const;
			std::fstream m_fs;
		};
#ifndef LIGHTNINGGE_INTERNAL_LOGGER
		LIGHTNINGGE_FOUNDATION_API Logger logger;
#endif
	}
} 