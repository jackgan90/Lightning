#include "Scene.h"
#include "IRenderer.h"
#include "RenderPlugin.h"
#include "Camera.h"

namespace Lightning
{
	namespace Scene
	{
		extern Plugins::RenderPlugin* gRenderPlugin;
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
			for (auto& drawable : mDrawables)
			{
				drawable->Draw(*renderer, mRenderData);
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