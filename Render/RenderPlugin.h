#pragma once
#include "Plugin.h"
#include "IMaterial.h"

namespace Lightning
{
	namespace Plugins
	{
		class RenderPlugin : public Plugin
		{
		public:
			virtual Render::IMaterial* CreateMaterial() = 0;
		};
	}
}