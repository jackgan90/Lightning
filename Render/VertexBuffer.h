#pragma once
#include <boost/functional/hash.hpp>
#include <memory>
#include "Container.h"
#include "IVertexBuffer.h"

namespace Lightning
{
	namespace Render
	{

		class VertexBuffer : public IVertexBuffer
		{
		public:
			VertexComponent INTERFACECALL GetVertexComponent(size_t index)const override;
			//get vertex attribute count associate with this vertex buffer
			std::size_t INTERFACECALL GetVertexComponentCount()const override;
			//get vertex size in bytes
			std::uint32_t INTERFACECALL GetVertexSize()const override;
			std::uint32_t INTERFACECALL GetBufferSize()const override;
			//get the buffer type
			GPUBufferType INTERFACECALL GetType()const override { return GPUBufferType::VERTEX; }
		protected:
			VertexBuffer(uint32_t bufferSize, const VertexDescriptor& descriptor);
			void CalculateVertexSize();
			VertexDescriptor mVertexDescriptor;
			std::uint32_t mVertexSize;
			std::uint32_t mBufferSize;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::VertexComponent>
	{
		size_t operator()(const Lightning::Render::VertexComponent& component)const noexcept
		{
			return component.GetHash();
		}
	};
}
