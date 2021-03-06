#include "ILoaderPlugin.h"
#include "Loader.h"
#include "Logger.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Loading;
		class LoaderPluginImpl : public ILoaderPlugin
		{
		public:
			LoaderPluginImpl(){}
			 ~LoaderPluginImpl()override;
			ILoader* GetLoader()override;
			void Tick()override;
			void OnCreated(IPluginManager*)override;
		private:
			IPluginManager* mPluginMgr;
		};

		void LoaderPluginImpl::OnCreated(IPluginManager* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Loader);
			LOG_INFO("Loader plugin init.");
			//Just to create the loader object.
			Loader::Instance();
		}

		LoaderPluginImpl::~LoaderPluginImpl()
		{
			LOG_INFO("Loader plugin unloaded!");
			Loader::Instance()->Finalize();
			FINALIZE_LOGGER(mPluginMgr)
		}

		void LoaderPluginImpl::Tick()
		{

		}

		ILoader* LoaderPluginImpl::GetLoader()
		{
			return Loader::Instance();
		}
	}
}

LIGHTNING_PLUGIN_IMPL(LoaderPluginImpl)
