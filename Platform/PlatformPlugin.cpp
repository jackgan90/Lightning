#include <memory>
#include "PlatformPlugin.h"
#include "Application/ApplicationFactory.h"
#include "PlatformExportDef.h"

namespace Lightning
{
	namespace Plugins
	{
		using App::ApplicationFactory;

		class PlatformPluginImpl : public PlatformPlugin
		{
		public:
			PlatformPluginImpl(const char* name);
			App::Application* CreateApplication()override;
		private:
			std::unique_ptr<App::Application> mApp;
		};

		PlatformPluginImpl::PlatformPluginImpl(const char* name) : PlatformPlugin(name)
		{

		}

		App::Application* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

extern "C"
{
	LIGHTNING_PLATFORM_API Lightning::Plugins::Plugin* GetPlugin(const char* name, Lightning::Plugins::IPluginMgr* mgr)
	{
		return new Lightning::Plugins::PlatformPluginImpl(name);
	}

	LIGHTNING_PLATFORM_API void ReleasePlugin(Lightning::Plugins::Plugin* pPlugin)
	{
		delete pPlugin;
	}

}
