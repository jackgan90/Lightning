#include "Scene.h"
#include "IRenderer.h"
#include "IRenderPlugin.h"
#include "IRenderable.h"
#include "Camera.h"

namespace Lightning
{
	namespace World
	{
		extern Plugins::IRenderPlugin* gRenderPlugin;
		Scene::Scene(const std::uint32_t id):mID(id)
		{
			mCameras.emplace_back(std::make_shared<Camera>());
		}

		Scene::~Scene()
		{
		}

		ISpaceCamera* Scene::GetActiveCamera()
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
				for (auto& spaceObject : mChildren)
				{
					if (auto renderable = std::dynamic_pointer_cast<IRenderable>(spaceObject))
					{
						renderable->Render(*renderer, camera);
					}
				}
			}
		}
	}
}