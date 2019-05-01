#pragma once
#include "Plugin.h"
#include "Application/IApplication.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IPlatformPlugin : public Plugin
		{
			virtual App::IApplication* CreateApplication() = 0;
		};
	}
}
