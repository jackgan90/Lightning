#include "Scene.h"
#include "IRenderer.h"
#include "IRenderPlugin.h"
#include "Camera.h"

namespace Lightning
{
	namespace Scene
	{
		extern Plugins::IRenderPlugin* gRenderPlugin;
		Scene::Scene(const std::uint32_t id):mID(id)
		{
			mActiveCamera = new Camera();
			mRenderData.camera = mActiveCamera;
		}

		Scene::~Scene()
		{
			if (mActiveCamera)
			{
				delete mActiveCamera;
				mActiveCamera = nullptr;
			}
		}

		void Scene::Tick()
		{
			auto renderer = gRenderPlugin->GetRenderer();
			if (mActiveCamera)
			{
				mActiveCamera->Tick(*renderer);
			}
			for (auto& drawable : mDrawables)
			{
				drawable->Draw(renderer, mRenderData);
			}
		}

		void Scene::AddDrawable(const std::shared_ptr<IDrawable>& drawable)
		{
			mDrawables.emplace_back(drawable);
		}

	}
}