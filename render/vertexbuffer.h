#pragma once
#include <vector>
#include <boost/functional/hash.hpp>
#include <memory>
#include "ivertexbuffer.h"
#include "gpubuffer.h"

namespace Lightning
{
	namespace Render
	{

		class LIGHTNING_RENDER_API VertexBuffer : public IVertexBuffer
		{
		public:
			const VertexComponent& GetComponentInfo(size_t attributeIndex)override;
			//get vertex attribute count associate with this vertex buffer
			std::uint8_t GetComponentCount()override;
			//get vertices count contained within this vertex buffer
			std::uint32_t GetVertexCount()const override;
			//get vertex size in bytes
			std::uint32_t GetVertexSize()const override;
			void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)override;
		protected:
			VertexBuffer(uint32_t bufferSize, const std::vector<VertexComponent>& components);
			void CalculateVertexSize();
			std::vector<VertexComponent> m_components;
			std::uint32_t m_vertexCount;
			std::uint32_t m_vertexSize;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::VertexComponent>
	{
		size_t operator()(const Lightning::Render::VertexComponent& component)const noexcept
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
