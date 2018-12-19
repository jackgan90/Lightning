#pragma once
#include <boost/functional/hash.hpp>
#include "Container.h"
#include "PlainObject.h"
#include "GPUBuffer.h"
#include "Semantics.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
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
			Container::Vector<VertexComponent> components;
		};

		class IVertexBuffer : public GPUBuffer
		{
		public:
			IVertexBuffer(std::uint32_t bufferSize):GPUBuffer(bufferSize){}
			virtual const VertexComponent& GetVertexComponent(size_t index) = 0;
			//get vertex attribute count associate with this vertex buffer
			virtual std::size_t GetVertexComponentCount() = 0;
			//get vertex size in bytes
			virtual std::uint32_t GetVertexSize()const = 0;
			GPUBufferType GetType()const override { return GPUBufferType::VERTEX; }
		};
	}
}
