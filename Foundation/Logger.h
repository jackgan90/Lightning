#pragma once
#include <string>
#include <fstream>
#include "Singleton.h"
#include "TimeSystem.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#ifdef _MSC_VER
#include "spdlog/sinks/msvc_sink.h"
#endif
#include "FoundationExportDef.h"

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
			~Logger()
			{
			}
			template<typename... Args>
			void Log(LogLevel level, const char* text, Args&&... args)
			{
				switch (level)
				{
				case LogLevel::Info:
					mLogger->info(text, std::forward<Args>(args)...);
					break;
				case LogLevel::Debug:
					mLogger->debug(text, std::forward<Args>(args)...);
					break;
				case LogLevel::Error:
					mLogger->error(text, std::forward<Args>(args)...);
					break;
				case LogLevel::Warning:
					mLogger->warn(text, std::forward<Args>(args)...);
					break;
				}
			}
			
			template<typename... Args>
			void Log(LogLevel level, const std::string& text, Args... args)
			{
				Log(level, text.c_str(), std::forward<Args>(args)...);
			}
		protected:
			Logger()
			{
				auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFileName, true);
#ifdef _MSC_VER
				auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
				mLogger = std::shared_ptr<spdlog::logger>(new spdlog::logger("Lightning", {fileSink, msvcSink}));
#else
				mLogger = std::shared_ptr<spdlog::logger>(new spdlog::logger("Lightning", { fileSink }));
#endif
				mLogger->set_level(spdlog::level::debug);
			}
		private:
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
			std::shared_ptr<spdlog::logger> mLogger;
			static constexpr char* LogFileName = "log.txt";
		};
	}
} 
