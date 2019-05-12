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
			mCameras.emplace_back(std::make_shared<Camera>());
		}

		Scene::~Scene()
		{
		}

		ICamera* Scene::GetActiveCamera()
		{
			if (mCameras.empty())
				return nullptr;
			return mCameras[0].get();
		}

		void Scene::Tick()
		{
			auto renderer = gRenderPlugin->GetRenderer();
			for (const auto& camera : mCameras)
			{
				auto window = renderer->GetOutputWindow();
				if (window)
				{
					auto width = window->GetWidth();
					auto height = window->GetHeight();
					auto aspectRatio = float(width) / height;
					camera->SetAspectRatio(aspectRatio);
				}
				for (auto& drawable : mDrawables)
				{
					drawable->Draw(renderer, camera);
				}
			}
		}

		void Scene::AddDrawable(const std::shared_ptr<IDrawable>& drawable)
		{
			mDrawables.emplace_back(drawable);
		}

	}
}