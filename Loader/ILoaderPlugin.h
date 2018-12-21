#pragma once
#include "IPlugin.h"
#include "ILoader.h"

namespace Lightning
{
	namespace Plugins
	{
		struct ILoaderPlugin : public IPlugin
		{
			virtual Loading::ILoader* INTERFACECALL GetLoader() = 0;
		};
	}
}