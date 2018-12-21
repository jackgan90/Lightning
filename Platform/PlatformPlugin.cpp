#include <memory>
#include "PlatformPlugin.h"
#include "Application/ApplicationFactory.h"
#include "IPluginManager.h"
#include "FoundationPlugin.h"
#include "Logger.h"
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		using App::ApplicationFactory;
		using App::IApplication;
		class PlatformPluginImpl : public PlatformPlugin
		{
		public:
			PlatformPluginImpl(){}
			~PlatformPluginImpl()override;
			IApplication* CreateApplication()override;
			void Update()override;
		protected:
			void OnCreated(IPluginManager*)override;
		private:
			std::unique_ptr<IApplication> mApp;
			IPluginManager* mPluginMgr;
			PLUGIN_OVERRIDE(PlatformPluginImpl)
		};

		void PlatformPluginImpl::OnCreated(IPluginManager* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Platform)
			LOG_INFO("Platform plugin init.");
		}

		PlatformPluginImpl::~PlatformPluginImpl()
		{
			LOG_INFO("Platform plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void PlatformPluginImpl::Update()
		{
			if (mApp)
				mApp->Update();
		}

		IApplication* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

LIGHTNING_PLUGIN_IMPL(PlatformPluginImpl)
