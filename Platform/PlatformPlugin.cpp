#include <memory>
#include "PlatformPlugin.h"
#include "Application/ApplicationFactory.h"

namespace Lightning
{
	namespace Plugins
	{
		using App::ApplicationFactory;

		class PlatformPluginImpl : public PlatformPlugin
		{
		public:
			App::Application* CreateApplication()override;
		private:
			std::unique_ptr<App::Application> mApp;
		};

		App::Application* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(PlatformPluginImpl)
