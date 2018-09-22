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
			void UpdateRenderItem();
			virtual std::uint8_t *GetVertices() = 0;
			virtual std::uint16_t *GetIndices() = 0;
			virtual Vector3f GetScale() = 0;
			virtual std::size_t GetVertexBufferSize() = 0;
			virtual std::size_t GetIndexBufferSize() = 0;
			Render::RenderItem mRenderItem;
			bool mFirstDraw;
		};

		struct PrimitiveDataSource
		{
			~PrimitiveDataSource()
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
			std::uint8_t *GetVertices() override { return reinterpret_cast<std::uint8_t*>(sPrototype.vertices); }
			std::uint16_t *GetIndices() override { return sPrototype.indices; }
			Vector3f GetScale() override { return Vector3f{mWidth, mHeight, mThickness}; }
			std::size_t GetVertexBufferSize() override { return sizeof(Vector3f) * 8; }
			std::size_t GetIndexBufferSize() override { return sizeof(std::uint16_t) * 36; }
			float mWidth;
			float mHeight;
			float mThickness;
			struct CubeDataSource : PrimitiveDataSource { CubeDataSource(); };
			static CubeDataSource sPrototype;
		};

		class LIGHTNING_SCENE_API Cylinder : public Primitive
		{
		public:
			Cylinder(float height, float radius);
			~Cylinder()override;
		protected:
			std::uint8_t *GetVertices() override { return reinterpret_cast<std::uint8_t*>(sPrototype.vertices); }
			std::uint16_t *GetIndices() override { return sPrototype.indices; }
			Vector3f GetScale() override { return Vector3f{mRadius, mHeight, mRadius}; }
			std::size_t GetVertexBufferSize() override { return sizeof(Vector3f) * VertexCount; }
			std::size_t GetIndexBufferSize() override { return sizeof(std::uint16_t) * IndexCount; }
			float mHeight;
			float mRadius;
			struct CylinderDataSource : PrimitiveDataSource { CylinderDataSource(); };
			static CylinderDataSource sPrototype;
		private:
			//mVertices include 74 vector3s upper circle + lower circle + 2 center
			static constexpr std::size_t VertexCount = 74;
			//a cylinder comprises of 144 triangles(36 on top, 36 on bottom, and 72 on body)
			static constexpr std::size_t IndexCount = 144 * 3;
			static constexpr std::size_t AngularUnit = 10;
		};
	}
}
