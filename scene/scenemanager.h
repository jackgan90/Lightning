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
				auto scene = new Scene(m_currentSceneId, std::forward<Args>(args)...);
				m_scenes[m_currentSceneId] = scene;
				m_currentSceneId++;
				return scene;
			}
			void Update();
			void DestroyScene(const std::uint32_t sceneId);
			void DestroyAll();
		protected:
			std::uint32_t m_currentSceneId;
			std::unordered_map<std::uint32_t, Scene*> m_scenes;
		};
	}
}
