#pragma once
#include <boost/functional/hash.hpp>
#include <memory>
#include "IVertexBuffer.h"

namespace Lightning
{
	namespace Render
	{

		class VertexBuffer : public IVertexBuffer
		{
		public:
			~VertexBuffer()override;
			//get vertex size in bytes
			std::uint32_t GetVertexSize()const override;
			const VertexDescriptor& GetVertexDescriptor()const override { return mVertexDescriptor; }
			std::uint32_t GetBufferSize()const override;
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
