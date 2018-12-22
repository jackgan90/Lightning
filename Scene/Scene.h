#pragma once
#include <cstdint>
#include "IScene.h"
#include "Container.h"
#include "SceneRenderData.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Container;
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
			Container::Vector<IDrawable*> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
