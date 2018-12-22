#pragma once
#include <cstdint>
#include "ICamera.h"
#include "IDrawable.h"

namespace Lightning
{
	namespace Scene
	{
		struct IScene
		{
			virtual INTERFACECALL ~IScene() = default;
			virtual std::uint32_t INTERFACECALL GetID()const = 0;
			virtual void INTERFACECALL Update() = 0;
			virtual void INTERFACECALL AddDrawable(IDrawable*) = 0;
			virtual ICamera* INTERFACECALL GetActiveCamera() = 0;
		};
	}
}