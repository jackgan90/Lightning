#pragma once
#include "Plugin.h"
#include "ILoader.h"

namespace Lightning
{
	namespace Plugins
	{
		struct ILoaderPlugin : public Plugin
		{
			virtual Loading::ILoader* GetLoader() = 0;
		};
	}
}