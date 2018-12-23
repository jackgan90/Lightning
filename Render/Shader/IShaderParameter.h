#pragma once
#include <cstddef>
#include "Portable.h"

namespace Lightning
{
	namespace Render
	{
		enum class ShaderParameterType
		{
			UNKNOWN,
			FLOAT,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			MATRIX4,
			TEXTURE,
			SAMPLER,
		};

		struct IShaderParameter
		{
			virtual INTERFACECALL ~IShaderParameter() = default;
			virtual const char* INTERFACECALL GetName()const = 0;
			virtual ShaderParameterType INTERFACECALL GetType()const = 0;
			virtual const void* INTERFACECALL Buffer(std::size_t& size)const = 0;
		};
	}
}

