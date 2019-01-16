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

		struct SamplerState
		{
			void Reset()
			{
				filterMode = SamplerFilterMode::Linear;
				addressU = addressV = addressW = AddressMode::Wrap;
				std::memset(borderColor, 0, sizeof(borderColor));
				mipLODBias = .0f;
				//Actually this value is used only when filter mode is anisotropic filtering.
				//And its min and max value is implementation dependent.
				maxAnisotropy = 0;
				minLOD = .0f;
				maxLOD = 9999999.0f;
			}
			SamplerFilterMode filterMode;
			AddressMode addressU;
			AddressMode addressV;
			AddressMode addressW;
			float borderColor[4];
			float mipLODBias;
			std::uint16_t maxAnisotropy;
			float minLOD;
			float maxLOD;
		};
		static_assert(std::is_pod<SamplerState>::value, "SamplerState is not a POD type.");
	}
}