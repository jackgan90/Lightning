#pragma once
#include "ISceneManager.h"
#include "Singleton.h"

namespace Lightning
{
	namespace World
	{
		using Foundation::Singleton;
		class SceneManager : public ISceneManager, public Singleton<SceneManager>
		{
		public:
			SceneManager();
			~SceneManager();
			IScene* CreateScene()override;
			IScene* GetForegroundScene()override;
			void SetForegroundScene(IScene* scene)override;
			void Tick()override;
			void DestroyScene(const std::uint32_t sceneId)override;
			void DestroyAllScenes()override;
		protected:
			void DestroyAllScenesImpl();
			std::uint32_t mCurrentSceneID;
			std::uint32_t mForegroundSceneID;
			std::unordered_map<std::uint32_t, std::unique_ptr<IScene>> mScenes;
		private:
			friend class Singleton<SceneManager>;
		};
	}
}
