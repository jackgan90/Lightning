#pragma once
#include <cstdint>
#include "PlainObject.h"

namespace Lightning
{
	namespace Render
	{
		enum class SamplerFilterMode
		{
			Point,
			Linear,
			Trilinear,
			Anisotropic,
		};

		enum class AddressMode
		{
			Wrap,
			Mirror,
			Clamp,
			Border
		};

		struct SamplerState : Foundation::PlainObject<SamplerState>
		{
			SamplerFilterMode filterMode;
			AddressMode addressU;
			AddressMode addressV;
			AddressMode addressW;
			float mipLODBias;
			std::uint16_t maxAnisotropy;
			float minLOD;
			float maxLOD;
		};
	}
}