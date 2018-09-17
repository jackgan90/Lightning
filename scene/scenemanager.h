#pragma once
#include <unordered_map>
#include "sceneexportdef.h"
#include "singleton.h"
#include "scene.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Singleton;
		class LIGHTNING_SCENE_API SceneManager : public Singleton<SceneManager>
		{
		public:
			~SceneManager();
			template<typename... Args>
			Scene* CreateScene(Args&&... args)
			{
				auto scene = new Scene(mCurrentSceneID, std::forward<Args>(args)...);
				mScenes[mCurrentSceneID] = scene;
				mCurrentSceneID++;
				return scene;
			}
			void Update();
			void DestroyScene(const std::uint32_t sceneId);
			void DestroyAll();
		protected:
			std::uint32_t mCurrentSceneID;
			std::unordered_map<std::uint32_t, Scene*> mScenes;
		};
	}
}
