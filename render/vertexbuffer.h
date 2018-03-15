#pragma once
#include <vector>
#include <boost/functional/hash.hpp>
#include <memory>
#include "ivertexbuffer.h"
#include "gpubuffer.h"

namespace LightningGE
{
	namespace Render
	{

		class LIGHTNINGGE_RENDER_API VertexBuffer : public IVertexBuffer
		{
		public:
			const VertexComponent& GetComponentInfo(size_t attributeIndex)override;
			//get vertex attribute count associate with this vertex buffer
			std::uint8_t GetComponentCount()override;
			//get vertices count contained within this vertex buffer
			std::uint32_t GetVertexCount()override;
			//get vertex size in bytes
			std::uint32_t GetVertexSize()override;
			void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)override;
		protected:
			VertexBuffer(const std::vector<VertexComponent>& components);
			void CalculateVertexCount();
			void CalculateVertexSize();
			std::vector<VertexComponent> m_components;
			std::uint32_t m_vertexCount;
			std::uint32_t m_vertexSize;
		private:
			bool m_vertexCountDirty;
			bool m_vertexSizeDirty;
		};
	}
}

namespace std
{
	template<> struct hash<LightningGE::Render::VertexComponent>
	{
		size_t operator()(const LightningGE::Render::VertexComponent& component)const noexcept
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