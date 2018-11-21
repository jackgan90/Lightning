#include "WindowPlugin.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"
#include "Logger.h"
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
			WindowPluginImpl(IPluginMgr* mgr);
			~WindowPluginImpl()override;
			Window::IWindow* NewWindow()override;
			void Update()override;
		private:
			IPluginMgr* mPluginMgr;
		};

		WindowPluginImpl::WindowPluginImpl(IPluginMgr* mgr):mPluginMgr(mgr)
		{
			INIT_LOGGER(mgr, Window)
			LOG_INFO("Window plugin init.");
		}

		WindowPluginImpl::~WindowPluginImpl()
		{
			LOG_INFO("Window plugin unloaded.");
			UNLOAD_FOUNDATION(mPluginMgr)
		}

		void WindowPluginImpl::Update()
		{

		}

		Window::IWindow* WindowPluginImpl::NewWindow()
		{
#ifdef LIGHTNING_WIN32
			return NEW_REF_OBJ(Window::WindowsGameWindow);
#endif
			return nullptr;
		}

	}
}

LIGHTNING_PLUGIN_IMPL(WindowPluginImpl)

