#pragma once
#include "IPlugin.h"
#include "Application/IApplication.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IPlatformPlugin : public Plugins::IPlugin
		{
			virtual App::IApplication* INTERFACECALL CreateApplication() = 0;
		};
	}
}
