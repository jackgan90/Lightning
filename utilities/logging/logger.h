#pragma once
#include <string>
#include <fstream>
#include "common.h"

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
			void Log(LogLevel level, std::string text);
		private:
			std::string LogLevelToPrefix(LogLevel level)const;
			std::fstream m_fs;
		};
	}
}
