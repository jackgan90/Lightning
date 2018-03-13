#pragma once
#include "enumoperation.h"

namespace LightningGE
{
	namespace Render
	{
		enum class RenderFormat
		{
			UNDEFINED,
			R32G32B32_FLOAT,
			R32G32B32A32_FLOAT,
			D24_S8,		//for depth-stencil buffer
			//TODO : there're more formats to add here...
		};

		enum class PrimitiveType
		{
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP
		};

		enum class DepthStencilClearFlags : unsigned int
		{
			CLEAR_DEPTH = 0x01,
			CLEAR_STENCIL = 0x02,
		};
		ENABLE_ENUM_BITMASK_OPERATORS(DepthStencilClearFlags)

		constexpr const int DEFAULT_SHADER_MODEL_MAJOR_VERSION = 5;
		constexpr const int DEFAULT_SHADER_MODEL_MINOR_VERSION = 0;
		constexpr const int MAX_GEOMETRY_BUFFER_COUNT = 8;
		constexpr const int RENDER_FRAME_COUNT = 3;
		constexpr const int MAX_RENDER_TARGET_COUNT = 8;
		const char* const DEFAULT_SHADER_ENTRY = "main";
	}
}