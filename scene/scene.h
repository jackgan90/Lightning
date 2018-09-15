#pragma once
#include <cstdint>
#include <vector>
#include "sceneexportdef.h"
#include "camera.h"
#include "drawable.h"
#include "scenerenderdata.h"

namespace Lightning
{
	namespace Scene
	{
		class LIGHTNING_SCENE_API Scene
		{
		public:
			Scene(std::uint32_t id);
			virtual ~Scene();
			std::uint32_t GetID()const { return m_id; }
			virtual void Update();
			void AddDrawable(const SharedDrawablePtr& drawable);
			Camera* GetMainCamera() { return m_mainCamera; }
		protected:
			Camera* m_mainCamera;
			std::uint32_t m_id;
			std::vector<SharedDrawablePtr> m_drawables;
			SceneRenderData m_renderData;
		};
	}
}
