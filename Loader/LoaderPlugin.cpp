#include "LoaderPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		class LoaderPluginImpl : public LoaderPlugin
		{
		public:
			Loading::Loader* GetLoader()override;
		};

		Loading::Loader* LoaderPluginImpl::GetLoader()
		{
			return Loading::Loader::Instance();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(LoaderPluginImpl)
