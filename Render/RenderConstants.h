#pragma once
#include <cstdint>
#include "EnumOperation.h"

namespace Lightning
{
	namespace Render
	{
		enum class RenderFormat : std::uint8_t
		{
			UNDEFINED,
			R32G32_FLOAT,
			R32G32B32_FLOAT,
			R32G32B32A32_FLOAT,
			R8G8B8A8_UNORM,
			D24_S8,		//for depth-stencil buffer
			//TODO : there're more formats to add here...
		};

		enum class PrimitiveType : std::uint8_t
		{
			POINT_LIST = 1,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP
		};

		enum class GPUBufferType
		{
			VERTEX,
			INDEX,
		};

		enum class IndexType
		{
			UINT16,
			UINT32,
		};

		enum class DepthStencilClearFlags : std::uint8_t
		{
			CLEAR_DEPTH = 0x01,
			CLEAR_STENCIL = 0x02,
		};
		ENABLE_ENUM_BITMASK_OPERATORS(DepthStencilClearFlags)

		constexpr std::uint8_t RENDER_FRAME_COUNT = 3;
		constexpr char* const DEFAULT_SHADER_ENTRY = "main";
	}
}