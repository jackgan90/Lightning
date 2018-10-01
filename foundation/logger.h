#pragma once
#include <string>
#include <fstream>
#ifdef _MSC_VER
#include <Windows.h>
//windows defines its min as a macro,that makes files include this header see min as the macro
//that's not the intent.at any rate when we need to use min semantic we should use std::min
#undef min
#endif
#include "singleton.h"
#include "timesystem.h"
#include "foundationexportdef.h"

#define LOG_INFO(text, ...)\
	Lightning::Foundation::Logger::Instance()->Log(Lightning::Foundation::LogLevel::Info, text, __VA_ARGS__)
#define LOG_DEBUG(text, ...)\
	Lightning::Foundation::Logger::Instance()->Log(Lightning::Foundation::LogLevel::Debug, text, __VA_ARGS__)
#define LOG_WARNING(text, ...)\
	Lightning::Foundation::Logger::Instance()->Log(Lightning::Foundation::LogLevel::Warning, text, __VA_ARGS__)
#define LOG_ERROR(text, ...)\
	Lightning::Foundation::Logger::Instance()->Log(Lightning::Foundation::LogLevel::Error, text, __VA_ARGS__)

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

		class LIGHTNING_FOUNDATION_API Logger : public Singleton<Logger>
		{
			friend class Singleton<Logger>;
		public:
			~Logger();
			template<typename... Args>
			void Log(LogLevel level, const std::string& text, Args... args)
			{
				//output to file
				auto prefix = LogLevelToPrefix(level);
				auto timeStr = Foundation::Time::GetCurrentTimeString();
				static char buf[4096];
#ifdef _MSC_VER
				sprintf_s(buf, text.c_str(), std::forward<Args>(args)...);
#else
				sprintf(buf, text.c_str(), std::forward<Args>(args)...);
#endif
				mFs << timeStr << " " << prefix << " " << buf << std::endl;
#ifdef _MSC_VER
				static char outputBuffer[4096];
				sprintf_s(outputBuffer, "%s %s %s\n", timeStr.c_str(), prefix.c_str(), buf);
				::OutputDebugString(outputBuffer);
#endif
			}
		protected:
			Logger();
		private:
			std::string LogLevelToPrefix(LogLevel level)const;
			std::fstream mFs;
		};
	}
} 
