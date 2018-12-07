#pragma once
#include <cstdint>
#include "RefCount.h"

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

		struct SamplerDescriptor
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

		class ISampler : Plugins::RefCount
		{

		};
	}
}