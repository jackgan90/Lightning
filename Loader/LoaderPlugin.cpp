#include "LoaderPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		class LoaderPluginImpl : public LoaderPlugin
		{
		public:
			Loading::Loader* GetLoader()override;
			~LoaderPluginImpl()override;
		};

		Loading::Loader* LoaderPluginImpl::GetLoader()
		{
			return Loading::Loader::Instance();
		}

		LoaderPluginImpl::~LoaderPluginImpl()
		{
			Loading::Loader::Instance()->Finalize();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(LoaderPluginImpl)
