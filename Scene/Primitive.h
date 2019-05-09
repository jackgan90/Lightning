#pragma once
#include <memory>
#include <cstdint>
#include "IDrawCall.h"
#include "IShader.h"
#include "IPrimitive.h"
#include "RefObject.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Transform;
		class Primitive : public IPrimitive
		{
		public:
			Primitive();
			~Primitive()override;
			void Draw(IRenderer* renderer, const SceneRenderData& sceneRenderData) override;
			PrimitiveType GetPrimitiveType()const override{ return mDrawCall->GetPrimitiveType(); }
			void SetWorldPosition(const Vector3f& pos) override{ mTransform.SetPosition(pos); }
			Vector3f GetWorldPosition()const override{ return mTransform.GetPosition(); }
			void SetWorldRotation(const Quaternionf& rot) override{ mTransform.SetRotation(rot); }
			Quaternionf GetWorldRotation()const override{ return mTransform.GetRotation(); }
			Color32 GetColor()const override{ return mColor; }
			void GetColor(float& a, float& r, float& g, float& b)override;
			void SetColor(const Color32& color)override;
			//color is in ARGB order
			void SetColor(std::uint32_t color)override;
			void SetColor(float a, float r, float g, float b)override;
			void SetTransparency(std::uint8_t transparency)override;
			void SetTransparency(float transparency)override;
			void SetTexture(const std::string& name, const std::shared_ptr<ITexture>& texture)override;
			void SetSamplerState(const std::string& name, const SamplerState& state)override;
			void SetShader(const std::shared_ptr<IShader>& shader)override;
		protected:
			virtual void UpdateDrawCall(IRenderer*);
			virtual std::uint8_t *GetVertices() = 0;
			virtual std::uint16_t *GetIndices() = 0;
			virtual Vector3f GetScale() = 0;
			virtual std::size_t GetVertexBufferSize() = 0;
			virtual std::size_t GetIndexBufferSize() = 0;
			Render::IDrawCall* mDrawCall;
			Transform mTransform;
			bool mDrawCallDirty;
			Color32 mColor;
			std::string mTextureName;
			std::shared_ptr<ITexture> mTexture;
			std::string mSamplerStateName;
			SamplerState mSamplerState;
			std::vector<std::shared_ptr<Render::IShader>> mShaders;
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
			void UpdateDrawCall(IRenderer*)override;
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
