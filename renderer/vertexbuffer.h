#pragma once
#include "ivertexbuffer.h"

namespace LightningGE
{
	namespace Renderer
	{
		class LIGHTNINGGE_RENDERER_API VertexBuffer : public IVertexBuffer
		{
		public:
			VertexBuffer(const std::vector<VertexComponent>& components);
			const VertexComponent& GetComponentInfo(size_t attributeIndex)override;
			//get vertex attribute count associate with this vertex buffer
			std::uint8_t GetComponentCount()override;
			//get internal data
			const std::uint8_t* GetBuffer()const override;
			//get internal buffer size in bytes
			std::uint32_t GetBufferSize()override;
			//get vertices count contained within this vertex buffer
			std::uint32_t GetVertexCount()override;
			//get vertex size in bytes
			std::uint32_t GetVertexSize()override;
			//set internal buffer,no copy
			void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)override;
			//set binding location for this vertex buffer.The buffer will be bound to loc-th slot when issue draw call
			void SetGPUBindSlot(std::uint16_t loc)override;
			//get binding location.Typically invoked by device to bind the buffer
			std::uint16_t GetGPUBindSlot()const override;
		protected:
			void CalculateVertexCount();
			void CalculateVertexSize();
			std::vector<VertexComponent> m_components;
			std::uint8_t* m_buffer;
			std::uint32_t m_bufferSize;
			std::uint16_t m_GPUBindSlot;
			std::uint32_t m_vertexCount;
			std::uint32_t m_vertexSize;
		private:
			bool m_vertexCountDirty;
			bool m_vertexSizeDirty;
		};
	}
}