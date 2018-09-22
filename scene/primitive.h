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
			PrimitiveType GetPrimitiveType()const { return mRenderItem.geometry->primType; }
		protected:
			Render::RenderItem mRenderItem;
			std::uint8_t *mVertices;
			std::uint16_t *mIndices;
		};

		class LIGHTNING_SCENE_API Cube : public Primitive
		{
		public:
			Cube(float size);
			~Cube()override;
		protected:
			float mSize;
			static float sVerticeTemplate[];
			static std::uint16_t sIndices[];
		};

		class LIGHTNING_SCENE_API Cylinder : public Primitive
		{
		public:
			Cylinder(float height, float radius);
			~Cylinder()override;
		protected:
			float mHeight;
			float mRadius;
		};
	}
}
