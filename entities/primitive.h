#pragma once
#include <memory>
#include <cstdint>
#include "drawable.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"

namespace LightningGE
{
	namespace Entities
	{
		enum PrimitiveType
		{
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP
		};

		class LIGHTNINGGE_ENTITIES_API Primitive : public IDrawable
		{
		public:
			Primitive(PrimitiveType topology);
			virtual ~Primitive();
			void Draw(Render::Renderer& renderer) override;
			PrimitiveType GetPrimitiveType()const { return m_primType; }
		protected:
			Render::SharedGeometryPtr m_geo;
			PrimitiveType m_primType;
		};

		class LIGHTNINGGE_ENTITIES_API Cube : public Primitive
		{
		public:
			Cube(float size);
			~Cube()override;
		protected:
			float m_size;
			std::uint8_t* m_vertices;
			static float verticeTemplate[24];
			static std::uint16_t s_indices[36];
		};
	}
}