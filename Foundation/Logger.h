#pragma once
#include <string>
#include <fstream>
#include "Singleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#ifdef _MSC_VER
#include "spdlog/sinks/msvc_sink.h"
#endif

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

		class Logger : public Singleton<Logger>
		{
		public:
			~Logger()
			{
			}
			template<typename... Args>
			void Log(LogLevel level, const char* text, Args&&... args)
			{
				assert(mLogger && "Logger is not initialized!Please call FoundationPlugin::InitLogger first!");
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
			void Init(const char* name, const std::shared_ptr<spdlog::sinks::basic_file_sink_mt>& fileSink
#ifdef _MSC_VER
				, const std::shared_ptr<spdlog::sinks::msvc_sink_mt>& msvcSink
#endif
			)
			{
#ifdef _MSC_VER
				spdlog::sinks_init_list sinks = { fileSink, msvcSink };
#else
				spdlog::sinks_init_list sinks = { fileSink };
#endif
				mLogger = std::make_unique<spdlog::logger>(name, sinks);
				mLogger->set_level(spdlog::level::debug);
			}
			void Finalize()
			{
				mLogger.reset();
			}
		private:
			friend class Singleton<Logger>;
			Logger() = default;
			std::unique_ptr<spdlog::logger> mLogger;
		};
	}
} 
