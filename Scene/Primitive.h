#pragma once
#include <memory>
#include <cstdint>
#include "IRenderUnit.h"
#include "IShader.h"
#include "IPrimitive.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Transformer;
		class Primitive : public IPrimitive
		{
		public:
			Primitive();
			INTERFACECALL ~Primitive()override;
			void INTERFACECALL Draw(IRenderer* renderer, const SceneRenderData& sceneRenderData) override;
			PrimitiveType INTERFACECALL GetPrimitiveType()const override{ return mRenderUnit->GetPrimitiveType(); }
			void INTERFACECALL SetWorldPosition(const Vector3f& pos) override{ mTransform.SetPosition(pos); }
			Vector3f INTERFACECALL GetWorldPosition()const override{ return mTransform.GetPosition(); }
			void INTERFACECALL SetWorldRotation(const Quaternionf& rot) override{ mTransform.SetRotation(rot); }
			Quaternionf INTERFACECALL GetWorldRotation()const override{ return mTransform.GetRotation(); }
			Color32 INTERFACECALL GetColor()const override{ return mColor; }
			void INTERFACECALL GetColor(float& a, float& r, float& g, float& b)override;
			void INTERFACECALL SetColor(const Color32& color)override;
			//color is in ARGB order
			void INTERFACECALL SetColor(std::uint32_t color)override;
			void INTERFACECALL SetColor(float a, float r, float g, float b)override;
			void INTERFACECALL SetTransparency(std::uint8_t transparency)override;
			void INTERFACECALL SetTransparency(float transparency)override;
			void INTERFACECALL SetTexture(const char* name, ITexture* texture)override;
			void INTERFACECALL SetSamplerState(const char* name, const SamplerState& state)override;
			void INTERFACECALL SetShader(IShader* shader)override;
		protected:
			virtual void UpdateRenderUnit(IRenderer*);
			virtual std::uint8_t *GetVertices() = 0;
			virtual std::uint16_t *GetIndices() = 0;
			virtual Vector3f GetScale() = 0;
			virtual std::size_t GetVertexBufferSize() = 0;
			virtual std::size_t GetIndexBufferSize() = 0;
			Render::IRenderUnit* mRenderUnit;
			Transformer mTransform;
			bool mShouldUpdateRenderUnit;
			Color32 mColor;
			std::string mTextureName;
			ITexture* mTexture;
			std::string mSamplerStateName;
			SamplerState mSamplerState;
			std::vector<Render::IShader*> mShaders;
			REF_OBJECT_OVERRIDE(Primitive)
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


		class Cube : public Primitive
		{
		public:
			Cube(float width = 1.0f, float height = 1.0f, float thickness = 1.0f);
		protected:
			void UpdateRenderUnit(IRenderer*)override;
			std::uint8_t *GetVertices() override { return sDataSource.vertices; }
			std::uint16_t *GetIndices() override { return sDataSource.indices; }
			Vector3f GetScale() override { return Vector3f{mWidth, mHeight, mThickness}; }
			std::size_t GetVertexBufferSize() override { return sizeof(Vector3f) * 48; }
			std::size_t GetIndexBufferSize() override { return sizeof(std::uint16_t) * 36; }
			float mWidth;
			float mHeight;
			float mThickness;
			struct CubeDataSource : PrimitiveDataSource { CubeDataSource(); };
			static CubeDataSource sDataSource;
		};

		class Cylinder : public Primitive
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

		class Hemisphere : public Primitive
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

		class Sphere : public Hemisphere
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
