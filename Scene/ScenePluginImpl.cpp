#include "IScenePlugin.h"
#include "SceneManager.h"
#include "Primitive.h"
#include "IPluginManager.h"
#include "IRenderPlugin.h"
#include "Logger.h"
#include "IFoundationPlugin.h"
#include "Plugin.h"

namespace Lightning
{
	using namespace Scene;
	namespace Scene
	{
		Plugins::IRenderPlugin* gRenderPlugin;
	}

	namespace Plugins
	{
		class ScenePluginImpl : public IScenePlugin
		{
		public:
			ScenePluginImpl() {};
			~ScenePluginImpl()override;
			ISceneManager* GetSceneManager()override;
			std::shared_ptr<IPrimitive> CreateCube(float width, float height, float thickness)override;
			std::shared_ptr<IPrimitive> CreateCylinder(float height, float radius)override;
			std::shared_ptr<IPrimitive> CreateHemisphere(float radius)override;
			std::shared_ptr<IPrimitive> CreateSphere(float radius)override;
			void Tick()override;
			void OnCreated(IPluginManager*)override;
		private:
			IPluginManager* mPluginMgr;
		};

		void ScenePluginImpl::OnCreated(IPluginManager* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Scene)
			Scene::gRenderPlugin = Plugins::GetPlugin<IRenderPlugin>(mgr, "Render");
			mgr->MakePlugin1UpdateBeforePlugin2(this, Scene::gRenderPlugin);
			LOG_INFO("Scene plugin init.");
		}

		ScenePluginImpl::~ScenePluginImpl()
		{
			LOG_INFO("Scene plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void ScenePluginImpl::Tick()
		{
			SceneManager::Instance()->Tick();
		}

		ISceneManager* ScenePluginImpl::GetSceneManager()
		{
			return SceneManager::Instance();
		}

		std::shared_ptr<IPrimitive> ScenePluginImpl::CreateCube(float width, float height, float thickness)
		{
			return std::make_shared<Cube>(width, height, thickness);
		}

		std::shared_ptr<IPrimitive> ScenePluginImpl::CreateCylinder(float height, float radius)
		{
			return std::make_shared<Cylinder>(height, radius);
		}

		std::shared_ptr<IPrimitive> ScenePluginImpl::CreateHemisphere(float radius)
		{
			return std::make_shared<Hemisphere>(radius);
		}

		std::shared_ptr<IPrimitive> ScenePluginImpl::CreateSphere(float radius)
		{
			return std::make_shared<Sphere>(radius);
		}
	}
}

LIGHTNING_PLUGIN_IMPL(ScenePluginImpl)
