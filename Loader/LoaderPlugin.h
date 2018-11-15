#pragma once
#include "PluginSystem/Plugin.h"
#include "Loader.h"

namespace Lightning
{
	namespace Plugins
	{
		class LoaderPlugin : public Plugin
		{
		public:
			virtual Loading::Loader* GetLoader() = 0;
		};
	}
}