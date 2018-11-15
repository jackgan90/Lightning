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
			PlatformPlugin(const char* name) : Plugin(name){}
			virtual App::Application* CreateApplication() = 0;
			virtual void DestroyApplication(App::Application* pApp) = 0;
		};
	}
}
