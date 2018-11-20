#include <memory>
#include "FoundationPlugin.h"
#include "ECS/EventManager.h"
#include "FileSystemFactory.h"
#include "IPluginMgr.h"
#include "ConfigManager.h"

namespace Lightning
{
	namespace Plugins
	{
		class FoundationPluginImpl : public FoundationPlugin
		{
		public:
			FoundationPluginImpl(IPluginMgr* mgr)
			{
				mFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFileName, true);
#ifdef _MSC_VER
				mMsvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#endif
				InitLoggerImpl("Foundation", Foundation::Logger::Instance());
				LOG_INFO("Foundation plugin init.");
			}
			~FoundationPluginImpl()override
			{
				LOG_INFO("Foundation plugin unloaded.");
				mFileSink->flush();
			}
			void InitLogger(const char* name, Foundation::Logger* logger)override;
			Foundation::IEventManager* GetEventManager()override;
			Foundation::IFileSystem* CreateFileSystem()override;
			Foundation::IConfigManager* GetConfigManager()override;
		private:
			void InitLoggerImpl(const char* name, Foundation::Logger* logger);
			std::shared_ptr<spdlog::sinks::basic_file_sink_mt> mFileSink;
#ifdef _MSC_VER
			std::shared_ptr<spdlog::sinks::msvc_sink_mt> mMsvcSink;
#endif
			static constexpr char* LogFileName = "F:\\Lightning_dev\\log.txt";
		};

		void FoundationPluginImpl::InitLogger(const char* name, Foundation::Logger* logger)
		{
			InitLoggerImpl(name, logger);
		}

		void FoundationPluginImpl::InitLoggerImpl(const char* name, Foundation::Logger* logger)
		{
#ifdef _MSC_VER
			logger->Init(name, mFileSink, mMsvcSink);
#else
			logger->Init(name, mFileSink);
#endif
		}

		Foundation::IEventManager* FoundationPluginImpl::GetEventManager()
		{
			return Foundation::EventManager::Instance();
		}

		Foundation::IFileSystem* FoundationPluginImpl::CreateFileSystem()
		{
			return Foundation::FileSystemFactory::Instance()->CreateFileSystem();
		}

		Foundation::IConfigManager* FoundationPluginImpl::GetConfigManager()
		{
			return Foundation::ConfigManager::Instance();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(FoundationPluginImpl)