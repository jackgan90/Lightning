#pragma once
#include <cstdint>
#include "ICamera.h"
#include "IDrawable.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::IDrawable;
		using Render::ICamera;
		struct IScene
		{
			virtual ~IScene() = default;
			virtual std::uint32_t GetID()const = 0;
			virtual void Tick() = 0;
			virtual void AddDrawable(const std::shared_ptr<IDrawable>&) = 0;
			virtual ICamera* GetActiveCamera() = 0;
		};
	}
}