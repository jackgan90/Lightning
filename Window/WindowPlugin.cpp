#include "WindowPlugin.h"
#include "IPluginManager.h"
#include "FoundationPlugin.h"
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
		class WindowPluginImpl : public WindowPlugin
		{
		public:
			WindowPluginImpl(){}
			~WindowPluginImpl()override;
			Window::IWindow* NewWindow()override;
			void Update()override;
		protected:
			void OnCreated(IPluginManager*)override;
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

