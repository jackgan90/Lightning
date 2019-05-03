#pragma once
#include <boost/functional/hash.hpp>
#include "PlainObject.h"
#include "IGPUBuffer.h"
#include "Semantics.h"

namespace Lightning
{
	namespace Render
	{
		struct VertexComponent
		{
			GET_HASH_METHOD
			void Reset()
			{
				semantic = POSITION;
				format = RenderFormat::R32G32B32_FLOAT;
				offset = 0;
				isInstance = false;
				instanceStepRate = 0;
			}
			RenderSemantics semantic;
			RenderFormat format;
			//relative to the start of a vertex
			unsigned int offset;
			//if this attribute is an instance attribute
			bool isInstance;
			//if this attribute is an instance attribute,specify after drawing how many instances should the attribute move to next value
			unsigned int instanceStepRate;
		};
		static_assert(std::is_pod<VertexComponent>::value, "VertexComponent is not a POD type.");

		struct VertexDescriptor
		{
			VertexComponent* components;
			std::size_t componentCount;
		};
		static_assert(std::is_pod<VertexDescriptor>::value, "VertexDescriptor is not a POD type.");

		struct IVertexBuffer : IGPUBuffer
		{
			//get vertex size in bytes
			virtual std::uint32_t GetVertexSize()const = 0;
			virtual const VertexDescriptor& GetVertexDescriptor()const = 0;
		};
	}
}
