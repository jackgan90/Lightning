#pragma once
#include "scenemanager.h"

namespace Lightning
{
	namespace Scene
	{
		void SceneManager::Update()
		{
			for (auto& scene : mScenes)
				scene.second->Update();
		}

		void SceneManager::DestroyScene(const std::uint32_t sceneId)
		{
			auto it = mScenes.find(sceneId);
			if (it != mScenes.end())
			{
				delete it->second;
				mScenes.erase(it);
			}
		}

		SceneManager::~SceneManager()
		{
			DestroyAll();
		}

		void SceneManager::DestroyAll()
		{
			for (auto it = mScenes.begin();it != mScenes.end();++it)
			{
				delete it->second;
			}
			mScenes.clear();
		}


	}
}