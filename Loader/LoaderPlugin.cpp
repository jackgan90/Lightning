#include "LoaderPlugin.h"
#include "Loader.h"
#include "Logger.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Loading;
		class LoaderPluginImpl : public LoaderPlugin
		{
		public:
			LoaderPluginImpl(IPluginMgr*);
			~LoaderPluginImpl()override;
			ILoader* GetLoader()override;
			void Update()override;
		private:
			IPluginMgr* mPluginMgr;
		};

		LoaderPluginImpl::LoaderPluginImpl(IPluginMgr* mgr) : mPluginMgr(mgr)
		{
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

		void LoaderPluginImpl::Update()
		{

		}

		ILoader* LoaderPluginImpl::GetLoader()
		{
			return Loader::Instance();
		}
	}
}

LIGHTNING_PLUGIN_IMPL(LoaderPluginImpl)
