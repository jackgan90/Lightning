#include "WindowPlugin.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"
#include "Logger.h"
#include "Container.h"
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
			~WindowPluginImpl()override;
			Window::IWindow* NewWindow()override;
			void Update()override;
		protected:
			void OnCreated(IPluginMgr*)override;
		private:
			IPluginMgr* mPluginMgr;
			Container::Vector<Window::IWindow*> mWindows;
		};

		void WindowPluginImpl::OnCreated(IPluginMgr* mgr)
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

