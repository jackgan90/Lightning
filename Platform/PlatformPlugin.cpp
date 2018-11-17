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
			auto foundation = mPluginMgr->Load<FoundationPlugin>("Foundation");
			foundation->InitLogger("Platform", Foundation::Logger::Instance());
		}

		PlatformPluginImpl::~PlatformPluginImpl()
		{
			mPluginMgr->Unload("Platform");
		}

		IApplication* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(PlatformPluginImpl)
