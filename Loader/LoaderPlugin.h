#pragma once
#include "Plugin.h"
#include "ILoader.h"

namespace Lightning
{
	namespace Plugins
	{
		class LoaderPlugin : public Plugin
		{
		public:
			virtual Loading::ILoader* GetLoader() = 0;
		};
	}
}