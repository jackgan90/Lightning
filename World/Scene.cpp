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
		Scene::Scene()
		{
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

		ISpaceCamera* Scene::CreateCamera()
		{
			auto camera = std::make_shared<Camera>();
			AddChild(camera);
			mCameras.emplace_back(camera);
			return camera.get();
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
				Traverse([this, renderer, &camera](const std::shared_ptr<ISpaceObject>& spaceObject) {
					if (auto renderable = std::dynamic_pointer_cast<IRenderable>(spaceObject))
					{
						renderable->Render(*renderer, camera);
					}
				}, SpaceObjectTraversalPolocy::Concurrent);
			}
		}
	}
}