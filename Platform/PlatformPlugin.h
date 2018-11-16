#pragma once
#include "PluginSystem/Plugin.h"
#include "Application/IApplication.h"
#include "Application/ApplicationFactory.h"

namespace Lightning
{
	namespace Plugins
	{
		class PlatformPlugin : public Plugins::Plugin
		{
		public:
			virtual App::IApplication* CreateApplication() = 0;
		};
	}
}
