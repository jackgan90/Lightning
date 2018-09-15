#pragma once
#include "scenemanager.h"

namespace Lightning
{
	namespace Scene
	{
		void SceneManager::Update()
		{
			for (auto& scene : m_scenes)
				scene.second->Update();
		}

		void SceneManager::DestroyScene(const std::uint32_t sceneId)
		{
			auto it = m_scenes.find(sceneId);
			if (it != m_scenes.end())
			{
				delete it->second;
				m_scenes.erase(it);
			}
		}

		SceneManager::~SceneManager()
		{
			DestroyAll();
		}

		void SceneManager::DestroyAll()
		{
			for (auto it = m_scenes.begin();it != m_scenes.end();++it)
			{
				delete it->second;
			}
			m_scenes.clear();
		}


	}
}