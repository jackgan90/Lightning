#include "scene.h"
#include "renderer.h"

namespace Lightning
{
	namespace Scene
	{
		Scene::Scene(const std::uint32_t id):m_id(id)
		{
			m_mainCamera = new Camera();
			m_renderData.camera = m_mainCamera;
		}

		Scene::~Scene()
		{
			if (m_mainCamera)
			{
				delete m_mainCamera;
				m_mainCamera = nullptr;
			}
		}

		void Scene::Update()
		{
			for (auto& drawable : m_drawables)
			{
				drawable->Draw(*Render::Renderer::Instance(), m_renderData);
			}
		}

		void Scene::AddDrawable(const SharedDrawablePtr& drawable)
		{
			m_drawables.push_back(drawable);
		}

	}
}