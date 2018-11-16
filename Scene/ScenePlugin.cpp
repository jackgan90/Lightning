#include "ScenePlugin.h"
#include "SceneManager.h"
#include "Primitive.h"
#include "IPluginMgr.h"
#include "RenderPlugin.h"

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
			ScenePluginImpl(IPluginMgr* mgr);
			ISceneManager* GetSceneManager()override;
			IPrimitive* CreateCube(float width, float height, float thickness)override;
			IPrimitive* CreateCylinder(float height, float radius)override;
			IPrimitive* CreateHemisphere(float radius)override;
			IPrimitive* CreateSphere(float radius)override;
		};

		ScenePluginImpl::ScenePluginImpl(IPluginMgr* mgr)
		{
			Scene::gRenderPlugin = mgr->Load<RenderPlugin>("Render");
		}

		ISceneManager* ScenePluginImpl::GetSceneManager()
		{
			return SceneManager::Instance();
		}

		IPrimitive* ScenePluginImpl::CreateCube(float width, float height, float thickness)
		{
			return new Cube(width, height, thickness);
		}

		IPrimitive* ScenePluginImpl::CreateCylinder(float height, float radius)
		{
			return new Cylinder(height, radius);
		}

		IPrimitive* ScenePluginImpl::CreateHemisphere(float radius)
		{
			return new Hemisphere(radius);
		}

		IPrimitive* ScenePluginImpl::CreateSphere(float radius)
		{
			return new Sphere(radius);
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(ScenePluginImpl)
