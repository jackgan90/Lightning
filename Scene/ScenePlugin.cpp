#include "ScenePlugin.h"
#include "SceneManager.h"
#include "Primitive.h"
#include "IPluginMgr.h"
#include "RenderPlugin.h"
#include "Logger.h"
#include "FoundationPlugin.h"

namespace Lightning
{
	using namespace Scene;
	namespace Scene
	{
		Plugins::RenderPlugin* gRenderPlugin;
	}

	namespace Plugins
	{
		class ScenePluginImpl : public ScenePlugin
		{
		public:
			~ScenePluginImpl()override;
			ISceneManager* GetSceneManager()override;
			IPrimitive* CreateCube(float width, float height, float thickness)override;
			IPrimitive* CreateCylinder(float height, float radius)override;
			IPrimitive* CreateHemisphere(float radius)override;
			IPrimitive* CreateSphere(float radius)override;
			void Update()override;
		protected:
			void OnCreated(IPluginMgr*)override;
		private:
			IPluginMgr* mPluginMgr;
		};

		void ScenePluginImpl::OnCreated(IPluginMgr* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Scene)
			Scene::gRenderPlugin = mgr->GetPlugin<RenderPlugin>("Render");
			mgr->MakePlugin1UpdateBeforePlugin2(this, Scene::gRenderPlugin);
			LOG_INFO("Scene plugin init.");
		}

		ScenePluginImpl::~ScenePluginImpl()
		{
			LOG_INFO("Scene plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void ScenePluginImpl::Update()
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
