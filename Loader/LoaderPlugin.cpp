#include "LoaderPlugin.h"
#include "Loader.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Loading;
		class LoaderPluginImpl : public LoaderPlugin
		{
		public:
			ILoader* GetLoader()override;
			~LoaderPluginImpl()override;
		};

		ILoader* LoaderPluginImpl::GetLoader()
		{
			return Loader::Instance();
		}

		LoaderPluginImpl::~LoaderPluginImpl()
		{
			Loader::Instance()->Finalize();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(LoaderPluginImpl)
