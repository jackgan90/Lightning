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
		using Window::IWindow;
		class WindowPluginImpl : public IWindowPlugin
		{
		public:
			WindowPluginImpl(){}
			~WindowPluginImpl()override;
			IWindow* CreateWindow()override;
			void Tick()override;
			void OnCreated(IPluginManager*)override;
		private:
			IPluginManager* mPluginMgr;
			std::vector<std::unique_ptr<IWindow>> mWindows;
		};

		void WindowPluginImpl::OnCreated(IPluginManager* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Window)
			LOG_INFO("Window plugin init.");
		}

		WindowPluginImpl::~WindowPluginImpl()
		{
			mWindows.clear();	//need to ensure the destructors of windows execute before finalize logger,because logger may be invoked in destructors.
			LOG_INFO("Window plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void WindowPluginImpl::Tick()
		{
			for (auto& window : mWindows)
			{
				window->Update();
			}
		}

		Window::IWindow* WindowPluginImpl::CreateWindow()
		{
#ifdef LIGHTNING_WIN32
			mWindows.push_back(std::make_unique<Window::WindowsGameWindow>());
			return mWindows.back().get();
#endif
			return nullptr;
		}

	}
}

LIGHTNING_PLUGIN_IMPL(WindowPluginImpl)

