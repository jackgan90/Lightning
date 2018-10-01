#pragma once
#include <boost/functional/hash.hpp>
#include <memory>
#include "container.h"
#include "ivertexbuffer.h"
#include "gpubuffer.h"

namespace Lightning
{
	namespace Render
	{

		class LIGHTNING_RENDER_API VertexBuffer : public IVertexBuffer
		{
		public:
			const VertexComponent& GetVertexComponent(size_t index)override;
			~VertexBuffer()override;
			//get vertex attribute count associate with this vertex buffer
			std::size_t GetVertexComponentCount()override;
			//get vertex size in bytes
			std::uint32_t GetVertexSize()const override;
		protected:
			VertexBuffer(uint32_t bufferSize, const VertexDescriptor& descriptor);
			void CalculateVertexSize();
			VertexDescriptor mVertexDescriptor;
			std::uint32_t mVertexSize;
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
