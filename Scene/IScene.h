#pragma once
#include <cstdint>
#include "ICamera.h"
#include "IDrawable.h"

namespace Lightning
{
	namespace Scene
	{
		class IScene
		{
		public:
			virtual ~IScene(){}
			virtual std::uint32_t GetID()const = 0;
			virtual void Update() = 0;
			virtual void AddDrawable(IDrawable*) = 0;
			virtual ICamera* GetActiveCamera() = 0;
		};
	}
}