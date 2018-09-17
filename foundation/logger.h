#pragma once
#include <string>
#include <fstream>
#ifdef _MSC_VER
#include <Windows.h>
//windows defines its min as a macro,that makes files include this header see min as the macro
//that's not the intent.at any rate when we need to use min semantic we should use std::min
#undef min
#endif
#include "timesystem.h"
#include "foundationexportdef.h"

namespace Lightning
{
	namespace Foundation
	{
		enum class LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		class LIGHTNING_FOUNDATION_API Logger
		{
		public:
			Logger();
			~Logger();
			template<typename... Args>
			void Log(LogLevel level, const std::string& text, Args... args)
			{
				//output to file
				auto prefix = LogLevelToPrefix(level);
				auto timeStr = Foundation::Time::GetCurrentTimeString();
				char buf[512];
				sprintf(buf, text.c_str(), std::forward<Args>(args)...);
				mFs << timeStr << " " << prefix << " " << buf << std::endl;
#ifdef _MSC_VER
				char outputBuffer[512];
				sprintf_s(outputBuffer, "%s %s %s\n", timeStr.c_str(), prefix.c_str(), buf);
				::OutputDebugString(outputBuffer);
#endif
			}
		private:
			std::string LogLevelToPrefix(LogLevel level)const;
			std::fstream mFs;
		};
#ifndef LIGHTNING_INTERNAL_LOGGER
		LIGHTNING_FOUNDATION_API Logger logger;
#endif
	}
} 
