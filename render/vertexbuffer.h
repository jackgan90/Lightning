#pragma once
#include <vector>
#include <boost/functional/hash.hpp>
#include <memory>
#include "semantics.h"
#include "bindablebuffer.h"
#include "renderconstants.h"

namespace LightningGE
{
	namespace Render
	{
		struct VertexComponent
		{
			SemanticItem semanticItem;
			SemanticIndex semanticIndex;
			RenderFormat format;
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

		class LIGHTNINGGE_RENDER_API VertexBuffer : public BindableBuffer
		{
		public:
			VertexBuffer(const std::vector<VertexComponent>& components);
			const VertexComponent& GetComponentInfo(size_t attributeIndex);
			//get vertex attribute count associate with this vertex buffer
			std::uint8_t GetComponentCount();
			//get vertices count contained within this vertex buffer
			std::uint32_t GetVertexCount();
			//get vertex size in bytes
			std::uint32_t GetVertexSize();
			void SetBuffer(std::uint8_t* buffer, std::uint32_t bufferSize)override;
		protected:
			void CalculateVertexCount();
			void CalculateVertexSize();
			std::vector<VertexComponent> m_components;
			std::uint32_t m_vertexCount;
			std::uint32_t m_vertexSize;
		private:
			bool m_vertexCountDirty;
			bool m_vertexSizeDirty;
		};
		using SharedVertexBufferPtr = std::shared_ptr<VertexBuffer>;
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