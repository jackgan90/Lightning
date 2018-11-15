#pragma once
#include "PlatformExportDef.h"
#include "PluginLoader/Plugin.h"
#ifdef LIGHTNING_WIN32
#include "Application/Win32AppSystem.h"
#endif

namespace Lightning
{
	namespace Plugins
	{
		class LIGHTNING_PLATFORM_API PlatformPlugin : public Plugin
		{
		public:
			PlatformPlugin(const char* name);
#ifdef LIGHTNING_WIN32
			App::Win32AppSystem* CreateWin32AppSystem();
#endif
		};
	}
}