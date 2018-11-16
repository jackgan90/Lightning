#pragma once
#include "ISceneManager.h"
#include "Container.h"
#include "Singleton.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Container;
		using Foundation::Singleton;
		class SceneManager : public ISceneManager, public Singleton<SceneManager>
		{
		public:
			SceneManager();
			~SceneManager();
			IScene* CreateScene()override;
			IScene* GetForegroundScene()override;
			void SetForegroundScene(IScene* scene)override;
			void Update()override;
			void DestroyScene(const std::uint32_t sceneId)override;
			void DestroyAllScenes()override;
			void OnFrameUpdate()override;
		protected:
			void DestroyAllScenesImpl();
			std::uint32_t mCurrentSceneID;
			Container::UnorderedMap<std::uint32_t, IScene*> mScenes;
			IScene* mForegroundScene;
		private:
			friend class Singleton<SceneManager>;
		};
	}
}
