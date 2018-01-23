#pragma once
#include <memory>
#include <cstdint>
#include "entitiesexportdef.h"
#include "renderer.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"

namespace LightningGE
{
	namespace Entities
	{
		enum PrimitiveTopology
		{
			PRIMITIVE_TOPOLOGY_POINT_LIST,
			PRIMITIVE_TOPOLOGY_LINE_LIST,
			PRIMITIVE_TOPOLOGY_LINE_STRIP,
			PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
		};

		class LIGHTNINGGE_ENTITIES_API Primitive
		{
		public:
			Primitive(PrimitiveTopology topology);
			virtual ~Primitive();
			void Draw(Render::Renderer& renderer);
			PrimitiveTopology GetTopology()const { return m_topology; }
		protected:
			std::unique_ptr<Render::VertexBuffer> m_vb;
			std::unique_ptr<Render::IndexBuffer> m_ib;
			PrimitiveTopology m_topology;
		};

		class LIGHTNINGGE_ENTITIES_API Cube : public Primitive
		{
		public:
			Cube(float size);
			~Cube()override;
		protected:
			struct CubeVertex
			{
				
			};
			float m_size;
			std::uint8_t* m_vertices;
			std::uint8_t* m_indices;
		};
	}
}