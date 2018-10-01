#pragma once
#include <boost/functional/hash.hpp>
#include "container.h"
#include "plainobject.h"
#include "gpubuffer.h"
#include "semantics.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		struct VertexComponent : Foundation::PlainObject<VertexComponent>
		{
			VertexComponent():
				semanticItem(EngineSemantics[0]),
				semanticIndex(0),
				format(RenderFormat::R32G32B32_FLOAT),
				offset(0),
				isInstance(false),
				instanceStepRate(0)
			{

			}
			SemanticItem semanticItem;
			SemanticIndex semanticIndex;
			RenderFormat format;
			//relative to the start of a vertex
			unsigned int offset;
			//if this attribute is an instance attribute
			bool isInstance;
			//if this attribute is an instance attribute,specify after drawing how many instances should the attribute move to next value
			unsigned int instanceStepRate;

		};

		struct VertexDescriptor
		{
			container::vector<VertexComponent> components;
		};

		class LIGHTNING_RENDER_API IVertexBuffer : public GPUBuffer
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
		using SharedVertexBufferPtr = std::shared_ptr<IVertexBuffer>;
	}
}
