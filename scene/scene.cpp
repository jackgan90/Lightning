#include "scene.h"
#include "renderer.h"

namespace Lightning
{
	namespace Scene
	{
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

		void Scene::Update()
		{
			for (auto& drawable : mDrawables)
			{
				drawable->Draw(*Render::Renderer::Instance(), mRenderData);
			}
		}

		void Scene::AddDrawable(const SharedDrawablePtr& drawable)
		{
			mDrawables.push_back(drawable);
		}

	}
}