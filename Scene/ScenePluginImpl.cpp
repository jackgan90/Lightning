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
			IPrimitive* CreateCube(float width, float height, float thickness)override;
			IPrimitive* CreateCylinder(float height, float radius)override;
			IPrimitive* CreateHemisphere(float radius)override;
			IPrimitive* CreateSphere(float radius)override;
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
			SceneManager::Instance()->Update();
		}

		ISceneManager* ScenePluginImpl::GetSceneManager()
		{
			return SceneManager::Instance();
		}

		IPrimitive* ScenePluginImpl::CreateCube(float width, float height, float thickness)
		{
			return NEW_REF_OBJ(Cube, width, height, thickness);
		}

		IPrimitive* ScenePluginImpl::CreateCylinder(float height, float radius)
		{
			return NEW_REF_OBJ(Cylinder, height, radius);
		}

		IPrimitive* ScenePluginImpl::CreateHemisphere(float radius)
		{
			return NEW_REF_OBJ(Hemisphere, radius);
		}

		IPrimitive* ScenePluginImpl::CreateSphere(float radius)
		{
			return NEW_REF_OBJ(Sphere, radius);
		}
	}
}

LIGHTNING_PLUGIN_IMPL(ScenePluginImpl)
