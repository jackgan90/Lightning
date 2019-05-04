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
			virtual ~IShaderParameter() = default;
			virtual std::string GetName()const = 0;
			virtual ShaderParameterType GetType()const = 0;
			virtual const void* Buffer(std::size_t& size)const = 0;
		};
	}
}

