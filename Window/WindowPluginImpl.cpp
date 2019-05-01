#include "IWindowPlugin.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"
#include "Logger.h"
#include "Plugin.h"
#ifdef LIGHTNING_WIN32
#include "WindowsGameWindow.h"
#endif
#undef CreateWindow

namespace Lightning
{
	namespace Plugins
	{
		using namespace Foundation;
		class WindowPluginImpl : public IWindowPlugin
		{
		public:
			WindowPluginImpl(){}
			~WindowPluginImpl()override;
			Window::IWindow* CreateWindow()override;
			void Tick()override;
			void OnCreated(IPluginManager*)override;
		private:
			IPluginManager* mPluginMgr;
			std::vector<Window::IWindow*> mWindows;
		};

		void WindowPluginImpl::OnCreated(IPluginManager* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Window)
			LOG_INFO("Window plugin init.");
		}

		WindowPluginImpl::~WindowPluginImpl()
		{
			for (auto window : mWindows)
			{
				window->Release();
			}
			LOG_INFO("Window plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void WindowPluginImpl::Tick()
		{
			for (auto window : mWindows)
			{
				window->Update();
			}
		}

		Window::IWindow* WindowPluginImpl::CreateWindow()
		{
#ifdef LIGHTNING_WIN32
			auto window = NEW_REF_OBJ(Window::WindowsGameWindow);
			window->AddRef();
			mWindows.push_back(window);
			return window;
#endif
			return nullptr;
		}

	}
}

LIGHTNING_PLUGIN_IMPL(WindowPluginImpl)

