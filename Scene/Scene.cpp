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
			for (auto drawable : mDrawables)
			{
				drawable->Release();
			}
		}

		void Scene::Update()
		{
			auto renderer = gRenderPlugin->GetRenderer();
			if (mActiveCamera)
			{
				mActiveCamera->Update(renderer);
			}
			for (auto& drawable : mDrawables)
			{
				drawable->Draw(renderer, mRenderData);
			}
		}

		void Scene::AddDrawable(IDrawable* drawable)
		{
			if (drawable)
			{
				drawable->AddRef();
				mDrawables.emplace_back(drawable);
			}
		}

	}
}