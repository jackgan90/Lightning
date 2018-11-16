#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"

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
			for (auto drawable : mDrawables)
			{
				drawable->Release();
			}
		}

		void Scene::Update()
		{
			for (auto& drawable : mDrawables)
			{
				drawable->Draw(*Render::Renderer::Instance(), mRenderData);
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