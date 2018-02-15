#include "scene.h"

namespace LightningGE
{
	namespace Scene
	{
		Scene::Scene(const std::uint32_t id):m_id(id)
		{
			m_mainCamera = new Camera();
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

		}

		void Scene::AddDrawable(const SharedDrawablePtr& drawable)
		{
			m_drawables.push_back(drawable);
		}

	}
}