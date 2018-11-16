#include "RenderPlugin.h"
#include "Material.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Render;
		class RenderPluginImpl : public RenderPlugin
		{
		public:
			RenderPluginImpl(IPluginMgr*){}
			IMaterial* CreateMaterial()override;
		};

		IMaterial* RenderPluginImpl::CreateMaterial()
		{
			return new Material();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(RenderPluginImpl)
