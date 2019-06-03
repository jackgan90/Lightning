#include "IWorldPlugin.h"
#include "SceneManager.h"
#include "Primitive.h"
#include "IPluginManager.h"
#include "IRenderPlugin.h"
#include "Logger.h"
#include "IFoundationPlugin.h"
#include "Plugin.h"
#include "SpaceObjectManager.h"

namespace Lightning
{
	using namespace World;
	namespace World
	{
		Plugins::IRenderPlugin* gRenderPlugin;
	}

	namespace Plugins
	{
		class WorldPluginImpl : public IWorldPlugin
		{
		public:
			WorldPluginImpl() {};
			~WorldPluginImpl()override;
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

		void WorldPluginImpl::OnCreated(IPluginManager* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, World)
			World::gRenderPlugin = Plugins::GetPlugin<IRenderPlugin>(mgr, "Render");
			mgr->MakePlugin1UpdateBeforePlugin2(this, World::gRenderPlugin);
			LOG_INFO("Scene plugin init.");
		}

		WorldPluginImpl::~WorldPluginImpl()
		{
			//Have to ensure the space objects are all flushed and corresponding destructors are called.
			SpaceObjectManager::Instance()->Synchronize();
			LOG_INFO("Scene plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void WorldPluginImpl::Tick()
		{
			SceneManager::Instance()->Tick();
			SpaceObjectManager::Instance()->Synchronize();
		}

		ISceneManager* WorldPluginImpl::GetSceneManager()
		{
			return SceneManager::Instance();
		}

		std::shared_ptr<IPrimitive> WorldPluginImpl::CreateCube(float width, float height, float thickness)
		{
			return std::make_shared<Cube>(width, height, thickness);
		}

		std::shared_ptr<IPrimitive> WorldPluginImpl::CreateCylinder(float height, float radius)
		{
			return std::make_shared<Cylinder>(height, radius);
		}

		std::shared_ptr<IPrimitive> WorldPluginImpl::CreateHemisphere(float radius)
		{
			return std::make_shared<Hemisphere>(radius);
		}

		std::shared_ptr<IPrimitive> WorldPluginImpl::CreateSphere(float radius)
		{
			return std::make_shared<Sphere>(radius);
		}
	}
}

LIGHTNING_PLUGIN_IMPL(WorldPluginImpl)
