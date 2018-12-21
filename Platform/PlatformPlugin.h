#pragma once
#include "IPlugin.h"
#include "Application/IApplication.h"

namespace Lightning
{
	namespace Plugins
	{
		struct PlatformPlugin : public Plugins::IPlugin
		{
			virtual App::IApplication* CreateApplication() = 0;
		};
	}
}
