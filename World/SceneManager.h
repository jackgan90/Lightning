#pragma once
#include <unordered_map>
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
			//Why don't we use unique_ptr here?Because objects of subclasses of SpaceObject must be referenced by a shared_ptr
			//(SpaceObject itself inherits std::enable_shared_from_this)
			std::unordered_map<std::uint32_t, std::shared_ptr<IScene>> mScenes;
		private:
			friend class Singleton<SceneManager>;
		};
	}
}
