#include "ScenePlugin.h"
#include "SceneManager.h"
#include "Primitive.h"

namespace Lightning
{
	using namespace Scene;
	namespace Plugins
	{
		class ScenePluginImpl : public ScenePlugin
		{
		public:
			ISceneManager* GetSceneManager()override;
			IPrimitive* CreateCube(float width, float height, float thickness)override;
			IPrimitive* CreateCylinder(float height, float radius)override;
			IPrimitive* CreateHemisphere(float radius)override;
			IPrimitive* CreateSphere(float radius)override;
		};

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