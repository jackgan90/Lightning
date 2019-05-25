#pragma once
#include <cstdint>
#include "ICamera.h"
#include "IRenderable.h"

namespace Lightning
{
	namespace World
	{
		using Render::ICamera;
		struct IScene
		{
			virtual ~IScene() = default;
			virtual std::uint32_t GetID()const = 0;
			virtual void Tick() = 0;
			virtual void AddRenderable(const std::shared_ptr<IRenderable>&) = 0;
			virtual ICamera* GetActiveCamera() = 0;
		};
	}
}