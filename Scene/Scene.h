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
			~Scene()override;
			std::uint32_t GetID()const override{ return mID; }
			void Update()override;
			void AddDrawable(IDrawable* )override;
			ICamera* GetActiveCamera()override { return mActiveCamera; }
		protected:
			ICamera* mActiveCamera;
			std::uint32_t mID;
			Container::Vector<std::unique_ptr<IDrawable>> mDrawables;
			SceneRenderData mRenderData;
		};
	}
}
