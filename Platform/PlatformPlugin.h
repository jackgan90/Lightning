#pragma once
#include "PluginSystem/Plugin.h"
#include "Application/Application.h"
#include "Application/ApplicationFactory.h"

namespace Lightning
{
	namespace Plugins
	{
		class PlatformPlugin : public Plugins::Plugin
		{
		public:
			virtual App::Application* CreateApplication() = 0;
		};
	}
}
