#pragma once
#include <memory>
#include <vector>
#include <functional>
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

		struct VertexAttribute
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
		};

		//note:IVertexBuffer assume tightly packed vertex data.
		class LIGHTNINGGE_RENDERER_API IVertexBuffer
		{
		public:
			virtual const VertexAttribute& GetAttributeInfo(size_t attributeIndex) = 0;
			//get vertex attribute count associate with this vertex buffer
			virtual unsigned int GetAttributeCount() = 0;
			//get internal data
			virtual const unsigned char* GetBuffer() = 0;
			//get internal buffer size in bytes
			virtual unsigned long GetBufferSize() = 0;
			//get vertices count contained within this vertex buffer
			virtual unsigned long GetVertexCount() = 0;
			//get vertex size in bytes
			virtual unsigned int GetVertexSize() = 0;
			//set internal buffer,no copy
			virtual void SetBuffer(const std::vector<VertexAttribute>& attributes, unsigned char* buffer, unsigned int bufferSize) = 0;
			//set binding location for this vertex buffer.The buffer will be bound to loc-th slot when issue draw call
			virtual void SetBindingLocation(int loc) = 0;
			//get binding location.Typically invoked by device to bind the buffer
			virtual int GetBindingLocation()const = 0;
		};
		typedef std::shared_ptr<IVertexBuffer> SharedVertexBufferPtr;
	}
}

namespace std
{
	template<> struct hash<LightningGE::Renderer::VertexAttribute>
	{
		size_t operator()(const LightningGE::Renderer::VertexAttribute& attribute)
		{
			size_t seed = 0;
			boost::hash_combine(seed, attribute.format);
			boost::hash_combine(seed, attribute.instanceStepRate);
			boost::hash_combine(seed, attribute.isInstance);
			boost::hash_combine(seed, attribute.offset);
			boost::hash_combine(seed, attribute.semanticIndex);
			boost::hash_combine(seed, attribute.semanticItem.semantic);
			return seed;
		}
	};
}