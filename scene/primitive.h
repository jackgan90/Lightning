#pragma once
#include <memory>
#include <cstdint>
#include "drawable.h"
#include "material.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "renderconstants.h"
#include "renderitem.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::PrimitiveType;
		class LIGHTNING_SCENE_API Primitive : public IDrawable
		{
		public:
			Primitive();
			virtual ~Primitive();
			void Draw(Render::Renderer& renderer, const SceneRenderData& sceneRenderData) override;
			PrimitiveType GetPrimitiveType()const { return m_renderItem.geometry->primType; }
		protected:
			Render::RenderItem m_renderItem;
		};

		class LIGHTNING_SCENE_API Cube : public Primitive
		{
		public:
			Cube(float size);
			~Cube()override;
		protected:
			float m_size;
			std::uint8_t* m_vertices;
			static float verticeTemplate[];
			static std::uint16_t s_indices[];
		};
	}
}
