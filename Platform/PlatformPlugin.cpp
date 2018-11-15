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
			void DestroyApplication(App::Application* pApp)override;
		};

		PlatformPluginImpl::PlatformPluginImpl(const char* name) : PlatformPlugin(name)
		{

		}

		App::Application* PlatformPluginImpl::CreateApplication()
		{
			return App::ApplicationFactory::CreateApplication();
		}

		void PlatformPluginImpl::DestroyApplication(App::Application* pApp)
		{
			delete pApp;
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
