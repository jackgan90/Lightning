#pragma once
#include "Plugin.h"
#include "Application/IApplication.h"

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
