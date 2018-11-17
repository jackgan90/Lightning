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
		private:
			IPluginMgr* mPluginMgr;
		};

		WindowPluginImpl::WindowPluginImpl(IPluginMgr* mgr):mPluginMgr(mgr)
		{
			auto foundation = mPluginMgr->Load<FoundationPlugin>("Foundation");
			foundation->InitLogger("Window", Foundation::Logger::Instance());
		}

		WindowPluginImpl::~WindowPluginImpl()
		{
			mPluginMgr->Unload("Foundation");
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

LIGHTNING_PLUGIN_INTERFACE(WindowPluginImpl)

