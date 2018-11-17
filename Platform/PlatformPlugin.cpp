#include <memory>
#include "PlatformPlugin.h"
#include "Application/ApplicationFactory.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"
#include "Logger.h"

namespace Lightning
{
	namespace Plugins
	{
		using App::ApplicationFactory;
		using App::IApplication;
		class PlatformPluginImpl : public PlatformPlugin
		{
		public:
			PlatformPluginImpl(IPluginMgr*);
			~PlatformPluginImpl()override;
			IApplication* CreateApplication()override;
		private:
			std::unique_ptr<IApplication> mApp;
			IPluginMgr* mPluginMgr;
		};

		PlatformPluginImpl::PlatformPluginImpl(IPluginMgr* mgr):mPluginMgr(mgr)
		{
			INIT_LOGGER(mgr, Platform)
			LOG_INFO("Platform plugin init.");
		}

		PlatformPluginImpl::~PlatformPluginImpl()
		{
			LOG_INFO("Platform plugin unloaded.");
			UNLOAD_FOUNDATION(mPluginMgr)
		}

		IApplication* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(PlatformPluginImpl)
