#include <PlatformPlugin.h>

namespace Lightning
{
	namespace Plugins
	{
		PlatformPlugin::PlatformPlugin(const char* name):Plugin(name)
		{

		}
#ifdef LIGHTNING_WIN32
		App::Win32AppSystem* PlatformPlugin::CreateWin32AppSystem()
		{
			return new App::Win32AppSystem;
		}
#endif

	}
}

extern "C"
{
	LIGHTNING_PLATFORM_API Lightning::Plugins::Plugin* GetPlugin(const char* name, Lightning::Plugins::PluginMgr* mgr)
	{
		return new Lightning::Plugins::PlatformPlugin(name);
	}
}
