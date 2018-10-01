#pragma once
#include <memory>
#include <cstdint>
#include "drawable.h"
#include "material.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "renderconstants.h"
#include "renderitem.h"
#include "color.h"

namespace Lightning
{
	namespace Scene
	{
		using Render::PrimitiveType;
		using Render::Color32;
		class LIGHTNING_SCENE_API Primitive : public IDrawable
		{
		public:
			Primitive();
			virtual ~Primitive();
			void Draw(Render::Renderer& renderer, const SceneRenderData& sceneRenderData) override;
			PrimitiveType GetPrimitiveType()const { return mRenderItem.geometry->primType; }
			void SetWorldPosition(const Vector3f& pos) { mWorldPosition = pos; }
			Vector3f GetWorldPosition()const { return mWorldPosition; }
			Color32 GetColor()const { return mColor; }
			void GetColor(float& a, float& r, float& g, float& b);
			void SetColor(const Color32& color);
			//color is in ARGB order
			void SetColor(std::uint32_t color);
			void SetColor(float a, float r, float g, float b);
			void SetTransparency(std::uint8_t transparency);
			void SetTransparency(float transparency);
		protected:
			void UpdateRenderItem();
			virtual std::uint8_t *GetVertices() = 0;
			virtual std::uint16_t *GetIndices() = 0;
			virtual Vector3f GetScale() = 0;
			virtual std::size_t GetVertexBufferSize() = 0;
			virtual std::size_t GetIndexBufferSize() = 0;
			Render::RenderItem mRenderItem;
			Vector3f mWorldPosition;
			bool mFirstDraw;
			Color32 mColor;
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
			std::uint8_t *vertices;
			std::uint16_t *indices;
		};


		class LIGHTNING_SCENE_API Cube : public Primitive
		{
		public:
			Cube(float width = 1.0f, float height = 1.0f, float thickness = 1.0f);
		protected:
			std::uint8_t *GetVertices() override { return sDataSource.vertices; }
			std::uint16_t *GetIndices() override { return sDataSource.indices; }
			Vector3f GetScale() override { return Vector3f{mWidth, mHeight, mThickness}; }
			std::size_t GetVertexBufferSize() override { return sizeof(Vector3f) * 16; }
			std::size_t GetIndexBufferSize() override { return sizeof(std::uint16_t) * 36; }
			float mWidth;
			float mHeight;
			float mThickness;
			struct CubeDataSource : PrimitiveDataSource { CubeDataSource(); };
			static CubeDataSource sDataSource;
		};

		class LIGHTNING_SCENE_API Cylinder : public Primitive
		{
		public:
			Cylinder(float height, float radius);
		protected:
			std::uint8_t *GetVertices() override { return sDataSource.vertices; }
			std::uint16_t *GetIndices() override { return sDataSource.indices; }
			Vector3f GetScale() override { return Vector3f{mRadius, mHeight, mRadius}; }
			std::size_t GetVertexBufferSize() override { return 2 * sizeof(Vector3f) * GetVertexCount(); }
			std::size_t GetIndexBufferSize() override { return sizeof(std::uint16_t) * GetIndexCount(); }
			float mHeight;
			float mRadius;
			struct CylinderDataSource : PrimitiveDataSource { CylinderDataSource(); };
			static CylinderDataSource sDataSource;
		private:
			static constexpr std::size_t GetVertexCount() { return 2 + 360 / AngularUnit * 2; }
			static constexpr std::size_t GetIndexCount() { return 360 / AngularUnit * 12; }
			static constexpr std::size_t AngularUnit{ 10 };
		};

		class LIGHTNING_SCENE_API Hemisphere : public Primitive
		{
		public:
			Hemisphere(float radius = 1.0f);
		protected:
			std::uint8_t *GetVertices() override { return sDataSource.vertices; }
			std::uint16_t *GetIndices() override { return sDataSource.indices; }
			Vector3f GetScale() override { return Vector3f{mRadius, mRadius, mRadius}; }
			std::size_t GetVertexBufferSize() override { return 2 * sizeof(Vector3f) * GetVertexCount(); }
			std::size_t GetIndexBufferSize() override { return sizeof(std::uint16_t) * GetIndexCount(); }
			float mRadius;
			struct HemisphereDataSource : PrimitiveDataSource { HemisphereDataSource(); };
			static HemisphereDataSource sDataSource;
			static constexpr std::size_t GetVertexCount() { 
				return 1 + (90 % AngularUnit == 0 ? 90 / AngularUnit : 90 / AngularUnit + 1) * 360 / AngularUnit; 
			}
			static constexpr std::size_t GetIndexCount(){
				return 6 * (90 % AngularUnit == 0 ? 90 / AngularUnit - 1 : 90 / AngularUnit) * 360 / AngularUnit + \
					3 * 360 / AngularUnit;
			}
		private:
			static constexpr std::size_t AngularUnit{ 10 };
		};

		class LIGHTNING_SCENE_API Sphere : public Hemisphere
		{
		public:
			Sphere(float radius = 1.0f);
		protected:
			std::uint8_t *GetVertices() override { return vertices; }
			std::uint16_t *GetIndices() override { return indices; }
			std::size_t GetVertexBufferSize() override { return Hemisphere::GetVertexBufferSize() * 2; }
			std::size_t GetIndexBufferSize() override { return Hemisphere::GetIndexBufferSize() * 2; }
			void InitVerticeAndIndice();
			static std::uint8_t *vertices;
			static std::uint16_t *indices;
		};
	}
}
