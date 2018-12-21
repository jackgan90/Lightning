#pragma once
#include "IPlugin.h"
#include "ILoader.h"

namespace Lightning
{
	namespace Plugins
	{
		struct LoaderPlugin : public IPlugin
		{
			virtual Loading::ILoader* GetLoader() = 0;
		};
	}
}