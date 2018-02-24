#pragma once
#include <memory>
#include <cstdint>
#include "drawable.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "renderconstants.h"

namespace LightningGE
{
	namespace Entities
	{
		using Render::PrimitiveType;
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