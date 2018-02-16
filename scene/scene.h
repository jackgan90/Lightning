#pragma once
#include <cstdint>
#include <vector>
#include "sceneexportdef.h"
#include "camera.h"
#include "drawable.h"

namespace LightningGE
{
	namespace Scene
	{
		using Entities::Camera;
		using Entities::SharedDrawablePtr;
		class LIGHTNINGGE_SCENE_API Scene
		{
		public:
			Scene(std::uint32_t id);
			virtual ~Scene();
			std::uint32_t GetID()const { return m_id; }
			virtual void Update();
			void AddDrawable(const SharedDrawablePtr& drawable);
		protected:
			Camera* m_mainCamera;
			std::uint32_t m_id;
			std::vector<SharedDrawablePtr> m_drawables;
		};
	}
}