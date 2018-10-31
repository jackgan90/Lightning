#pragma once
#include "scenemanager.h"
#include "renderer.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::Renderer;
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

		SceneManager::SceneManager():mForegroundScene(nullptr)
		{
			Renderer::Instance()->RegisterCallback(Render::RendererEvent::FRAME_UPDATE, [this]() {OnFrameUpdate(); });
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

		Scene* SceneManager::GetForegroundScene()
		{
			return mForegroundScene;
		}

		void SceneManager::SetForegroundScene(Scene* scene)
		{
			mForegroundScene = scene;
		}
		
		void SceneManager::OnFrameUpdate()
		{
			Update();
		}
	}
}