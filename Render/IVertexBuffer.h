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
		struct VertexComponent : Foundation::PlainObject<VertexComponent>
		{
			RenderSemantics semantic{ POSITION };
			RenderFormat format{ RenderFormat::R32G32B32_FLOAT };
			//relative to the start of a vertex
			unsigned int offset{ 0 };
			//if this attribute is an instance attribute
			bool isInstance{ false };
			//if this attribute is an instance attribute,specify after drawing how many instances should the attribute move to next value
			unsigned int instanceStepRate{ 0 };

		};

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
