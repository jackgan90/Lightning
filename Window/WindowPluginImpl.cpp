#include "IWindowPlugin.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"
#include "Logger.h"
#include "Container.h"
#include "Plugin.h"
#ifdef LIGHTNING_WIN32
#include "WindowsGameWindow.h"
#endif

namespace Lightning
{
	namespace Plugins
	{
		using namespace Foundation;
		class WindowPluginImpl : public IWindowPlugin
		{
		public:
			WindowPluginImpl(){}
			INTERFACECALL ~WindowPluginImpl()override;
			Window::IWindow* INTERFACECALL NewWindow()override;
			void INTERFACECALL Update()override;
			void INTERFACECALL OnCreated(IPluginManager*)override;
		private:
			IPluginManager* mPluginMgr;
			Container::Vector<Window::IWindow*> mWindows;
			PLUGIN_OVERRIDE(WindowPluginImpl)
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

		void WindowPluginImpl::Update()
		{
			for (auto window : mWindows)
			{
				window->Update();
			}
		}

		Window::IWindow* WindowPluginImpl::NewWindow()
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
