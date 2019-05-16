#pragma once
#include <type_traits>
#include <cstdint>

namespace Lightning
{
	namespace World
	{
		struct MeshHeader
		{
			std::uint32_t Magic;
			std::uint8_t VertexAttributes;		//number of vertex attributes
			std::uint64_t PositionOffset;		//point to the start of vertex position data,measured in offset from file begin
			std::uint64_t NormalOffset;			//point to the start of normal data, measured in offset from file begin, 0 if not present
			std::uint64_t TangentOffset;		//point to the start of tangent data, measured in offset from file begin, 0 if not present
		};
		
		static_assert(std::is_pod<MeshHeader>::value, "MeshHeader must be a POD type.");
	}
}