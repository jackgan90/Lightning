#pragma once
#include "SceneManager.h"
#include "IRenderer.h"
#include "RenderPlugin.h"
#include "Scene.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IRenderer;
		extern Plugins::RenderPlugin* gRenderPlugin;
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

		}

		SceneManager::~SceneManager()
		{
			DestroyAllScenesImpl();
		}

		void SceneManager::DestroyAllScenes()
		{
			DestroyAllScenesImpl();
		}

		void SceneManager::DestroyAllScenesImpl()
		{
			for (auto it = mScenes.begin();it != mScenes.end();++it)
			{
				delete it->second;
			}
			mScenes.clear();
		}

		IScene* SceneManager::CreateScene()
		{
			auto scene = new Scene(mCurrentSceneID);
			mScenes[mCurrentSceneID] = scene;
			mCurrentSceneID++;
			if (!GetForegroundScene())
				SetForegroundScene(scene);
			return scene;
		}

		IScene* SceneManager::GetForegroundScene()
		{
			return mForegroundScene;
		}

		void SceneManager::SetForegroundScene(IScene* scene)
		{
			mForegroundScene = scene;
		}
	}
}