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
		private:
			IPluginMgr* mPluginMgr;
		};

		LoaderPluginImpl::LoaderPluginImpl(IPluginMgr* mgr) : mPluginMgr(mgr)
		{
			auto foundation = mPluginMgr->Load<FoundationPlugin>("Foundation");
			foundation->InitLogger("Loader", Foundation::Logger::Instance());
		}

		LoaderPluginImpl::~LoaderPluginImpl()
		{
			Loader::Instance()->Finalize();
			mPluginMgr->Unload("Foundation");
		}

		ILoader* LoaderPluginImpl::GetLoader()
		{
			return Loader::Instance();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(LoaderPluginImpl)
