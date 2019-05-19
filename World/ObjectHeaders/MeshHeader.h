#pragma once
#include <type_traits>
#include <cstdint>

namespace Lightning
{
	namespace World
	{
		//Maximum number of vertex streams.Note position/normal/tangent/bitangent also accounts to stream count.
		//eg.Position + Normal + Tangents + Bitangents + Other vertex attributes cannot exceed MaxVertexStreams.
		constexpr std::uint8_t MaxVertexStreams = 16u;

		struct VertexStream
		{
			std::uint8_t Components;
			std::uint32_t Offset;
			std::uint32_t Count;
		};

		static_assert(std::is_pod<VertexStream>::value, "VertexStream must be a POD type.");

		struct MeshHeader
		{
			std::uint32_t Magic;
			std::uint32_t Version;
			std::uint8_t PrimitiveType;		//Triangle?Or other types of primitives
			std::uint8_t VertexStreams;		//number of vertex streams
			std::uint8_t IndexSize;			
			std::uint32_t IndexOffset;
			std::uint32_t IndexCount;
			VertexStream Positions;			//point to the start of vertex position data,measured in offset from file begin
			VertexStream Normals;			//point to the start of normal data, measured in offset from file begin, 0 if not present
			VertexStream Tangents;			//point to the start of tangent data, measured in offset from file begin, 0 if not present
			VertexStream Bitagents;			
			VertexStream VertexAttributes[MaxVertexStreams];
		};
		
		static_assert(std::is_pod<MeshHeader>::value, "MeshHeader must be a POD type.");
	}
}