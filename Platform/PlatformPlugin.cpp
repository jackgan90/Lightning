#include <memory>
#include "PlatformPlugin.h"
#include "Application/ApplicationFactory.h"

namespace Lightning
{
	namespace Plugins
	{
		using App::ApplicationFactory;
		using App::IApplication;
		class PlatformPluginImpl : public PlatformPlugin
		{
		public:
			IApplication* CreateApplication()override;
		private:
			std::unique_ptr<IApplication> mApp;
		};

		IApplication* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(PlatformPluginImpl)
