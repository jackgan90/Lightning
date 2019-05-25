#pragma once
#include <cstdint>
#include <vector>
#include "IScene.h"

namespace Lightning
{
	namespace World
	{
		class Scene : public IScene
		{
		public:
			Scene(std::uint32_t id);
			~Scene()override;
			std::uint32_t GetID()const override{ return mID; }
			void Tick()override;
			void AddRenderable(const std::shared_ptr<IRenderable>& drawable)override;
			ICamera* GetActiveCamera()override;
		protected:
			std::vector<std::shared_ptr<ICamera>> mCameras;
			std::uint32_t mID;
			std::vector<std::shared_ptr<IRenderable>> mRenderables;
		};
	}
}
