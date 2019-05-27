#pragma once
#include "SceneManager.h"
#include "IRenderer.h"
#include "IRenderPlugin.h"
#include "Scene.h"

namespace Lightning
{
	namespace World
	{
		using Render::IRenderer;
		extern Plugins::IRenderPlugin* gRenderPlugin;
		void SceneManager::Tick()
		{
			for (auto& scene : mScenes)
				scene.second->Tick();
		}

		void SceneManager::DestroyScene(const std::uint32_t sceneId)
		{
			auto it = mScenes.find(sceneId);
			if (it != mScenes.end())
			{
				mScenes.erase(it);
			}
		}

		SceneManager::SceneManager()
			: mForegroundSceneID(0)
			, mCurrentSceneID(1)
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
			mScenes.clear();
		}

		IScene* SceneManager::CreateScene()
		{
			auto sceneId = mCurrentSceneID++;
			mScenes[sceneId] = std::make_shared<Scene>(sceneId);
			mScenes[sceneId]->CreateCamera();
			if (!GetForegroundScene())
				SetForegroundScene(mScenes[sceneId].get());
			return mScenes[sceneId].get();
		}

		IScene* SceneManager::GetForegroundScene()
		{
			auto it = mScenes.find(mForegroundSceneID);
			if (it != mScenes.end())
			{
				return it->second.get();
			}
			return nullptr;
		}

		void SceneManager::SetForegroundScene(IScene* scene)
		{
			if (scene)
			{
				mForegroundSceneID = scene->GetID();
			}
		}
	}
}