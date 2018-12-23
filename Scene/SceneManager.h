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
			INTERFACECALL ~SceneManager();
			IScene* INTERFACECALL CreateScene()override;
			IScene* INTERFACECALL GetForegroundScene()override;
			void INTERFACECALL SetForegroundScene(IScene* scene)override;
			void INTERFACECALL Update()override;
			void INTERFACECALL DestroyScene(const std::uint32_t sceneId)override;
			void INTERFACECALL DestroyAllScenes()override;
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
