#pragma once
#include "ivertexbuffer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API VertexBuffer : public IVertexBuffer
		{
		public:
			VertexBuffer();
			const VertexAttribute& GetAttributeInfo(size_t attributeIndex)override;
			//get vertex attribute count associate with this vertex buffer
			unsigned int GetAttributeCount()override;
			//get internal data
			const unsigned char* GetBuffer()override;
			//get internal buffer size in bytes
			unsigned long GetBufferSize()override;
			//get vertices count contained within this vertex buffer
			unsigned long GetVertexCount()override;
			//get vertex size in bytes
			unsigned int GetVertexSize()override;
			//set internal buffer,no copy
			void SetBuffer(const std::vector<VertexAttribute>& attributes, unsigned char* buffer, unsigned int bufferSize)override;
			//set binding location for this vertex buffer.The buffer will be bound to loc-th slot when issue draw call
			void SetBindingLocation(int loc)override;
			//get binding location.Typically invoked by device to bind the buffer
			int GetBindingLocation()const override;
		protected:
			void CalculateVertexCount();
			void CalculateVertexSize();
			std::vector<VertexAttribute> m_attributes;
			unsigned char* m_buffer;
			unsigned long m_bufferSize;
			unsigned int m_bindingLocation;
			unsigned int m_vertexCount;
			unsigned int m_vertexSize;
		private:
			bool m_vertexCountDirty;
			bool m_vertexSizeDirty;
		};
	}
}