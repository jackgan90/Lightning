#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include "rendererexportdef.h"
#include "semantics.h"

namespace LightningGE
{
	namespace Renderer
	{
		enum VertexFormat
		{
			VERTEX_FORMAT_R32G32B32_FLOAT,
			//TODO : there're more formats to add here...
		};

		struct VertexComponent
		{
			SemanticItem semanticItem;
			SemanticIndex semanticIndex;
			VertexFormat format;
			//relative to the start of a vertex
			unsigned int offset;
			//if this attribute is an instance attribute
			bool isInstance;
			//if this attribute is an instance attribute,specify after drawing how many instances should the attribute move to next value
			unsigned int instanceStepRate;

			bool operator==(const VertexComponent& component)const noexcept
			{
				if (semanticItem.semantic != component.semanticItem.semantic)
				{
					return false;
				}

				if (semanticIndex != component.semanticIndex)
				{
					return false;
				}

				if (format != component.format)
				{
					return false;
				}

				if (offset != component.offset)
				{
					return false;
				}

				if (isInstance != component.isInstance)
				{
					return false;
				}

				if (instanceStepRate != component.instanceStepRate)
				{
					return false;
				}

				return true;
			}

			bool operator!=(const VertexComponent& component)const noexcept
			{
				return !(*this == component);
			}

		};

		//note:IVertexBuffer assume tightly packed vertex data.
		class LIGHTNINGGE_RENDERER_API IVertexBuffer
		{
		public:
			virtual const VertexComponent& GetComponentInfo(size_t componentIndex) = 0;
			//get vertex attribute count associate with this vertex buffer
			virtual std::uint8_t GetComponentCount() = 0;
			//get internal data
			virtual const std::uint8_t* GetBuffer()const = 0;
			//get internal buffer size in bytes
			virtual std::uint32_t GetBufferSize() = 0;
			//get vertices count contained within this vertex buffer
			virtual std::uint32_t GetVertexCount() = 0;
			//get vertex size in bytes
			virtual std::uint32_t GetVertexSize() = 0;
			//set internal buffer,no copy
			virtual void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize) = 0;
			//set binding location for this vertex buffer.The buffer will be bound to loc-th slot when issue draw call
			virtual void SetGPUBindSlot(std::uint16_t loc) = 0;
			//get binding location.Typically invoked by device to bind the buffer
			virtual std::uint16_t GetGPUBindSlot()const = 0;
		};
		typedef std::shared_ptr<IVertexBuffer> SharedVertexBufferPtr;
	}
}

namespace std
{
	template<> struct hash<LightningGE::Renderer::VertexComponent>
	{
		size_t operator()(const LightningGE::Renderer::VertexComponent& component)const noexcept
		{
			size_t seed = 0;
			boost::hash_combine(seed, component.format);
			boost::hash_combine(seed, component.instanceStepRate);
			boost::hash_combine(seed, component.isInstance);
			boost::hash_combine(seed, component.offset);
			boost::hash_combine(seed, component.semanticIndex);
			boost::hash_combine(seed, component.semanticItem.semantic);
			return seed;
		}
	};
}