#include "WindowPlugin.h"
#include "IPluginMgr.h"
#ifdef LIGHTNING_WIN32
#include "WindowsGameWindow.h"
#endif

namespace Lightning
{
	namespace Plugins
	{
		class WindowPluginImpl : public WindowPlugin
		{
		public:
			WindowPluginImpl(IPluginMgr* mgr){}
			Window::IWindow* NewWindow()override;
		};

		Window::IWindow* WindowPluginImpl::NewWindow()
		{
#ifdef LIGHTNING_WIN32
			return new Window::WindowsGameWindow();
#endif
			return nullptr;
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(WindowPluginImpl)

