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

		struct PrimitivePrototype
		{
			~PrimitivePrototype()
			{
				if (vertices)
					delete[] vertices;
				if (indices)
					delete[] indices;
			}
			float *vertices;
			std::uint16_t *indices;
		};


		class LIGHTNING_SCENE_API Cube : public Primitive
		{
		public:
			Cube(float width = 1.0f, float height = 1.0f, float thickness = 1.0f);
			~Cube()override;
		protected:
			float mWidth;
			float mHeight;
			float mThickness ;
			struct CubePrototype : PrimitivePrototype { CubePrototype(); };
			static CubePrototype sPrototype;
		};

		class LIGHTNING_SCENE_API Cylinder : public Primitive
		{
		public:
			Cylinder(float height, float radius);
			~Cylinder()override;
		protected:
			float mHeight;
			float mRadius;
			struct CylinderPrototype : PrimitivePrototype { CylinderPrototype(); };
			static CylinderPrototype sPrototype;
		private:
			//mVertices include 74 vector3s upper circle + lower circle + 2 center
			static constexpr std::size_t VertexCount = 74;
			//a cylinder comprises of 144 triangles(36 on top, 36 on bottom, and 72 on body)
			static constexpr std::size_t IndexCount = 144 * 3;
			static constexpr std::size_t AngularUnit = 10;
		};
	}
}
