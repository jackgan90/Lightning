#pragma once
#include <cstdint>
#include "IScene.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		class Scene : public IScene
		{
		public:
			Scene(std::uint32_t id);
			INTERFACECALL ~Scene()override;
			std::uint32_t INTERFACECALL GetID()const override{ return mID; }
			void INTERFACECALL Update()override;
			void INTERFACECALL AddDrawable(IDrawable* )override;
			ICamera* INTERFACECALL GetActiveCamera()override { return mActiveCamera; }
		protected:
			ICamera* mActiveCamera;
			std::uint32_t mID;
			std::vector<IDrawable*> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
