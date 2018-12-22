#include <memory>
#include "IFoundationPlugin.h"
#include "ECS/EventManager.h"
#include "FileSystemFactory.h"
#include "IPluginManager.h"
#include "ConfigManager.h"
#include "Environment.h"
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		class FoundationPluginImpl : public IFoundationPlugin
		{
		public:
			FoundationPluginImpl()
			{
				mFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFileName, true);
#ifdef _MSC_VER
				mMsvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#endif
				InitLoggerImpl("Foundation", Foundation::Logger::Instance());
				LOG_INFO("Foundation plugin init.");
				mFileSystem = Foundation::FileSystemFactory::Instance()->CreateFileSystem();
				LOG_INFO("File system created!Current working directory:{0}", mFileSystem->GetRoot());
			}
			INTERFACECALL ~FoundationPluginImpl()override
			{
				Foundation::FileSystemFactory::Instance()->DestroyFileSystem(mFileSystem);
				LOG_INFO("Foundation plugin unloaded.");
				mFileSink->flush();
			}
			void INTERFACECALL Update()override;
			void INTERFACECALL InitLogger(const char* name, Foundation::Logger* logger)override;
			void INTERFACECALL FinalizeLogger(Foundation::Logger* logger)override;
			Foundation::IEventManager* INTERFACECALL GetEventManager()override;
			Foundation::IFileSystem* INTERFACECALL GetFileSystem()override;
			Foundation::IConfigManager* INTERFACECALL GetConfigManager()override;
			Foundation::IEnvironment* INTERFACECALL GetEnvironment()override;
			void INTERFACECALL OnCreated(IPluginManager*)override;
		private:
			void InitLoggerImpl(const char* name, Foundation::Logger* logger);
			std::shared_ptr<spdlog::sinks::basic_file_sink_mt> mFileSink;
			Foundation::IFileSystem* mFileSystem;
#ifdef _MSC_VER
			std::shared_ptr<spdlog::sinks::msvc_sink_mt> mMsvcSink;
#endif
			static constexpr char* LogFileName = "log.txt";
			PLUGIN_OVERRIDE(FoundationPluginImpl)
		};

		void FoundationPluginImpl::OnCreated(IPluginManager* mgr)
		{

		}

		void FoundationPluginImpl::Update()
		{

		}

		void FoundationPluginImpl::InitLogger(const char* name, Foundation::Logger* logger)
		{
			InitLoggerImpl(name, logger);
		}

		void FoundationPluginImpl::FinalizeLogger(Foundation::Logger* logger)
		{
			logger->Finalize();
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

		Foundation::IFileSystem* FoundationPluginImpl::GetFileSystem()
		{
			return mFileSystem;
		}

		Foundation::IConfigManager* FoundationPluginImpl::GetConfigManager()
		{
			return Foundation::ConfigManager::Instance();
		}

		Foundation::IEnvironment* FoundationPluginImpl::GetEnvironment()
		{
			return Foundation::Environment::Instance();
		}
	}
}

LIGHTNING_PLUGIN_IMPL(FoundationPluginImpl)
