#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Common.h"
#include "Container.h"
#include "Math/Vector.h"
#include "Primitive.h"
#include "IPluginManager.h"
#include "IRenderPlugin.h"
#include "ShaderParameter.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}
	namespace Scene
	{
		using Foundation::Math::Vector2f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		using Foundation::Math::DegreesToRadians;
		using Foundation::Container;
		using Render::ShaderParameter;
		extern Plugins::IRenderPlugin* gRenderPlugin;

		Primitive::Primitive():mShouldUpdateRenderUnit(true)
			,mColor{0, 0, 0, 255}, mTexture(nullptr), mRenderUnit(nullptr)
		{

		}

		Primitive::~Primitive()
		{
			if (mTexture)
				mTexture->Release();
			for (auto shader : mShaders)
			{
				shader->Release();
			}
			if (mRenderUnit)
				mRenderUnit->Release();
		}

		void Primitive::SetColor(const Color32& color)
		{
			mColor = color;
		}

		void Primitive::SetColor(std::uint32_t color)
		{
			mColor.a = static_cast<std::uint8_t>((color & 0xff000000) >> 24);
			mColor.r = static_cast<std::uint8_t>((color & 0x00ff0000) >> 16);
			mColor.g = static_cast<std::uint8_t>((color & 0x0000ff00) >> 8);
			mColor.b = static_cast<std::uint8_t>(color & 0x000000ff);
		}

		void Primitive::SetColor(float a, float r, float g, float b)
		{
			mColor.a = static_cast<std::uint8_t>(a * 255);
			mColor.r = static_cast<std::uint8_t>(r * 255);
			mColor.g = static_cast<std::uint8_t>(g * 255);
			mColor.b = static_cast<std::uint8_t>(b * 255);
		}

		void Primitive::GetColor(float& a, float& r, float& g, float& b)
		{
			a = float(mColor.a) / 255;
			r = float(mColor.r) / 255;
			g = float(mColor.g) / 255;
			b = float(mColor.b) / 255;
		}

		void Primitive::SetTransparency(std::uint8_t transparency)
		{
			mColor.a = transparency;
		}

		void Primitive::SetTransparency(float transparency)
		{
			mColor.a = static_cast<std::uint8_t>(255 * transparency);
		}

		void Primitive::SetTexture(const char* name, ITexture* texture)
		{
			if (texture != mTexture)
			{
				if (texture)
					texture->AddRef();
				mTextureName = name;
				mTexture = texture;
				mShouldUpdateRenderUnit = true;
			}
		}

		void Primitive::SetSamplerState(const char* name, const SamplerState& state)
		{
			mSamplerStateName = name;
			mSamplerState = state;
			mShouldUpdateRenderUnit = true;
		}

		void Primitive::SetShader(IShader* shader)
		{
			if (shader == nullptr)
				return;
			shader->AddRef();
			bool replaced{ false };
			for(auto i = 0;i < mShaders.size();++i)
			{
				auto s = mShaders[i];
				if (s->GetType() == shader->GetType())
				{
					s->Release();
					mShaders[i] = shader;
					replaced = true;
					break;
				}
			}
			if (!replaced)
				mShaders.push_back(shader);
		}

		void Primitive::Draw(IRenderer* renderer, const SceneRenderData& sceneRenderData)
		{
			if (mShouldUpdateRenderUnit)
			{
				UpdateRenderUnit(renderer);
				mShouldUpdateRenderUnit = false;
			}
			if (sceneRenderData.camera)
			{
				mRenderUnit->SetViewMatrix(sceneRenderData.camera->GetViewMatrix());
				mRenderUnit->SetProjectionMatrix(sceneRenderData.camera->GetProjectionMatrix());
			}
			renderer->CommitRenderUnit(mRenderUnit);
		}

		void Primitive::UpdateRenderUnit(IRenderer* renderer)
		{
			if (mRenderUnit)
				mRenderUnit->Release();
			mRenderUnit = renderer->CreateRenderUnit();
			Render::VertexDescriptor descriptor;
			Container::Vector<Render::VertexComponent> components;
			Render::VertexComponent compPosition;
			compPosition.format = Render::RenderFormat::R32G32B32_FLOAT;
			compPosition.instanceStepRate = 0;
			compPosition.isInstance = false;
			compPosition.offset = 0;
			compPosition.semantic = Render::RenderSemantics::POSITION;
			components.push_back(compPosition);
			Render::VertexComponent compNormal;
			compNormal.format = Render::RenderFormat::R32G32B32_FLOAT;
			compNormal.instanceStepRate = 0;
			compNormal.isInstance = false;
			compNormal.offset = sizeof(float) * 3;
			compNormal.semantic = Render::RenderSemantics::NORMAL;
			components.push_back(compNormal);

			auto pDevice = renderer->GetDevice();
			auto vbSize = GetVertexBufferSize();
			auto ibSize = GetIndexBufferSize();
			descriptor.components = &components[0];
			descriptor.componentCount = components.size();
			auto vertexBuffer = pDevice->CreateVertexBuffer(static_cast<std::uint32_t>(vbSize), descriptor);
			mRenderUnit->SetVertexBuffer(0, vertexBuffer);
			auto indexBuffer = pDevice->CreateIndexBuffer(static_cast<std::uint32_t>(ibSize), Render::IndexType::UINT16);
			mRenderUnit->SetIndexBuffer(indexBuffer);
			
			
			auto vertices = GetVertices();

			auto indices = GetIndices();

			auto mem = vertexBuffer->Lock(0, vbSize);
			std::memcpy(mem, vertices, vbSize);
			vertexBuffer->Unlock(0, vbSize);

			mem = indexBuffer->Lock(0, ibSize);
			std::memcpy(mem, indices, ibSize);
			indexBuffer->Unlock(0, ibSize);
			vertexBuffer->Release();
			indexBuffer->Release();

			mRenderUnit->SetPrimitiveType(Render::PrimitiveType::TRIANGLE_LIST);
			
			auto material = gRenderPlugin->CreateMaterial();
			IShader *vs{ nullptr }, *ps{ nullptr };
			for (auto shader : mShaders)
			{
				if (shader->GetType() == Render::ShaderType::VERTEX)
					vs = shader;
				if (shader->GetType() == Render::ShaderType::FRAGMENT)
					ps = shader;
				material->SetShader(shader);
			}
			if (!vs)
			{
				vs = pDevice->GetDefaultShader(Render::ShaderType::VERTEX);
				vs->AddRef();
				mShaders.push_back(vs);
				material->SetShader(vs);
			}
			if (!ps)
			{
				ps = pDevice->GetDefaultShader(Render::ShaderType::FRAGMENT);
				ps->AddRef();
				mShaders.push_back(ps);
				material->SetShader(ps);
			}
			float a, r, g, b;
			GetColor(a, r, g, b);
			ShaderParameter paramColor("color", Vector4f{ r, g, b, a });
			ShaderParameter paramLight("light", Vector3f{ 3, 3, 3 });
			material->SetParameter(Render::ShaderType::FRAGMENT, &paramColor);
			material->SetParameter(Render::ShaderType::FRAGMENT, &paramLight);

			material->EnableBlend(mColor.a != 0xff);
			mRenderUnit->SetMaterial(material);
			material->Release();
			mTransform.SetScale(GetScale());
			mRenderUnit->SetTransform(mTransform.GetTransform());
		}

		
		//Cube
		Cube::CubeDataSource::CubeDataSource()
		{
			auto pVecs = new Vector3f[16];
			pVecs[0] = { 0.5f, 0.5f, 0.5f };	//right top front v0
			pVecs[1] = { 0.5774f, 0.5774f, 0.5774f };
			pVecs[2] = { 0.5f, 0.5f, -0.5f };	//right top back v1
			pVecs[3] = { 0.5774f, 0.5774f, -0.5774f };
			pVecs[4] = { 0.5f, -0.5f, 0.5f };	//right bottom front v2
			pVecs[5] = { 0.5774f, -0.5774f, 0.5774f };
			pVecs[6] = { 0.5f, -0.5f, -0.5f };	//right bottom back v3
			pVecs[7] = { 0.5774f, -0.5774f, -0.5774f };
			pVecs[8] = { -0.5f, 0.5f, 0.5f };	//left top front v4
			pVecs[9] = { -0.5774f, 0.5774f, 0.5774f };
			pVecs[10] = { -0.5f, 0.5f, -0.5f };	//left top back v5
			pVecs[11] = { -0.5774f, 0.5774f, -0.5774f };
			pVecs[12] = { -0.5f, -0.5f, 0.5f };	//left bottom front v6
			pVecs[13] = { -0.5774f, -0.5774f, 0.5774f };
			pVecs[14] = { -0.5f, -0.5f, -0.5f };	//left bottom back v7
			pVecs[15] = { -0.5774f, -0.5774f, -0.5774f };
			vertices = reinterpret_cast<std::uint8_t*>(pVecs);
			std::uint16_t index_data[] = {
				//right face
				0, 2, 3, 0, 3, 1,
				//front face
				4, 6, 2, 4, 2, 0,
				//left face
				5, 7, 6, 5, 6, 4,
				//back face
				1, 3, 7, 1, 7, 5,
				//top face
				5, 4, 0, 5, 0, 1,
				//bottom face
				6, 7, 3, 6, 3, 2
			};
			indices = new std::uint16_t[36];
			std::memcpy(indices, index_data, 36 * sizeof(std::uint16_t));
		}

		Cube::CubeDataSource Cube::sDataSource;
		Cube::Cube(float width, float height, float thickness) : 
			mWidth(width), mHeight(height), mThickness(thickness)
		{
			assert(mWidth > 0 && mHeight > 0 && mThickness > 0 && "The size of the cube must be greater than 0!");
		}

		void Cube::UpdateRenderUnit(IRenderer* renderer)
		{
			Primitive::UpdateRenderUnit(renderer);
			if (mTexture)
			{
				//generate uv
				Render::VertexDescriptor descriptor;
				Container::Vector<Render::VertexComponent> components;
				Render::VertexComponent compTexcoord;
				compTexcoord.format = Render::RenderFormat::R32G32_FLOAT;
				compTexcoord.instanceStepRate = 0;
				compTexcoord.isInstance = false;
				compTexcoord.offset = 0;
				compTexcoord.semantic = Render::RenderSemantics::TEXCOORD0;
				components.push_back(compTexcoord);
				auto pDevice = renderer->GetDevice();
				auto uvBufferSize = sizeof(Vector2f) * 8;
				descriptor.components = &components[0];
				descriptor.componentCount = components.size();
				auto vertexBuffer = pDevice->CreateVertexBuffer(static_cast<std::uint32_t>(uvBufferSize), descriptor);
				mRenderUnit->SetVertexBuffer(1, vertexBuffer);
				auto uv = reinterpret_cast<Vector2f*>(vertexBuffer->Lock(0, uvBufferSize));
				uv[0].x = 0.0f; uv[0].y = 0.0f; //right top front v0
				uv[1].x = 1.0f; uv[1].y = 0.0f; //right top back v1
				uv[2].x = 0.0f; uv[2].y = 1.0f; //right bottom front v2
				uv[3].x = 1.0f; uv[3].y = 1.0f; //right bottom back v3
				uv[4].x = 1.0f; uv[4].y = 0.0f; //left top front v4
				uv[5].x = 0.0f; uv[5].y = 0.0f; //left top back v5
				uv[6].x = 1.0f; uv[6].y = 1.0f; //left bottom front v6
				uv[7].x = 0.0f; uv[7].y = 1.0f; //left bottom back v7
				vertexBuffer->Unlock(0, uvBufferSize);
				vertexBuffer->Release();
				//apply texture to material
			}
		}
		//Cube

		//Cylinder
		Cylinder::CylinderDataSource::CylinderDataSource()
		{
			static const auto CirclePointCount = 360 / AngularUnit;
			auto vertexCount = GetVertexCount();
			auto indexCount = GetIndexCount();
			Vector3f *data = new Vector3f[2 * vertexCount];
			indices = new std::uint16_t[indexCount];
			std::memset(indices, 0, sizeof(std::uint16_t) * indexCount);
			
			Vector3f& topCenter = data[0];
			topCenter.x = 0;
			topCenter.y = static_cast<float>(1.0 / 2.0);
			topCenter.z = 0;
			data[1] = { 0, 1, 0 };

			Vector3f& bottomCenter = data[2 * vertexCount - 2];
			bottomCenter.x = 0;
			bottomCenter.y = static_cast<float>(-1.0 / 2.0);
			bottomCenter.z = 0;
			data[2 * vertexCount - 1] = { 0, -1, 0 };

			int j = 0, k = 2;
			for (int i = 0;i < CirclePointCount;++i)
			{
				//upper circle
				auto radians = DegreesToRadians(i * AngularUnit);
				//normal
				data[k + 1] = Vector3f::right() * std::cos(radians) + Vector3f::back() * std::sin(radians);
				data[k] = topCenter + 0.5f * data[k + 1];
				
				data[k + 1 + 2 * CirclePointCount] = data[k + 1];
				data[k + 2 * CirclePointCount] = data[k] + Vector3f::down();
				k += 2;

				auto isLast = i == CirclePointCount - 1;
				
				//top triangle
				indices[j++] = 0;
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2);
				else
					indices[j++] = static_cast<std::uint16_t>(1);
					

				//bottom triangle
				indices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
				indices[j++] = static_cast<std::uint16_t>(vertexCount - 1);
				if (!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
				else
					indices[j++] = static_cast<std::uint16_t>(1 + CirclePointCount);

				//side triangle 0
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
				{
					indices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
					indices[j++] = static_cast<std::uint16_t>(i + 2);
				}
				else
				{
					indices[j++] = static_cast<std::uint16_t>(1 + CirclePointCount);
					indices[j++] = static_cast<std::uint16_t>(1);
				}

				//side triangle 1
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				indices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
				if(!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
				else
					indices[j++] = static_cast<std::uint16_t>(1 + CirclePointCount);
			}
			assert(j == indexCount && "Not all indices are initialized!");
			vertices = reinterpret_cast<std::uint8_t*>(data);
		}

		Cylinder::CylinderDataSource Cylinder::sDataSource;
		Cylinder::Cylinder(float height, float radius) :mHeight(height), mRadius(radius)
		{
			assert(height > 0 && radius > 0 && "height and radius of a cylinder must be positive!");
		}
		//Cylinder

		//Hemisphere
		Hemisphere::HemisphereDataSource Hemisphere::sDataSource;
		Hemisphere::Hemisphere(float radius) :mRadius(radius)
		{
			assert(radius > 0 && "radius must be positive");
		}

		Hemisphere::HemisphereDataSource::HemisphereDataSource()
		{
			static const auto CirclePointCount = 360 / AngularUnit;
			auto vertexCount = GetVertexCount();
			auto indexCount = GetIndexCount();
			Vector3f *data = new Vector3f[2 * vertexCount];
			indices = new std::uint16_t[indexCount];
			std::memset(indices, 0, sizeof(std::uint16_t) * indexCount);
			data[2 * vertexCount - 2].x = 0;
			data[2 * vertexCount - 2].y = 0.5;
			data[2 * vertexCount - 2].z = 0;
			data[2 * vertexCount - 1] = { 0, 1, 0 };
			auto ringCount = 90 % AngularUnit == 0 ? 90 / AngularUnit : 90 / AngularUnit + 1;
			std::size_t k = 0, t = 0;
			for (std::size_t i = 0;i < ringCount;++i)
			{
				auto y_radians = DegreesToRadians(i * AngularUnit);
				auto y = 0.5f * std::sin(y_radians);
				auto radius_at_y = 0.5f * std::cos(y_radians);
				for (std::size_t j = 0;j < CirclePointCount;++j)
				{
					auto xz_radians = DegreesToRadians(j * AngularUnit);
					auto index = i * CirclePointCount + j;
					//pos
					data[t++] = y * Vector3f::up() + radius_at_y * 
						(Vector3f::right() * std::cos(xz_radians) + Vector3f::back() * std::sin(xz_radians));
					//normal
					data[t] = data[t - 1] * 2.0f;
					++t;
					if (i == ringCount - 1)
					{
						indices[k++] = static_cast<std::uint16_t>(vertexCount - 1);
						indices[k++] = static_cast<std::uint16_t>(index);
						if (j < CirclePointCount - 1)
							indices[k++] = static_cast<std::uint16_t>(index + 1);
						else
							indices[k++] = static_cast<std::uint16_t>(index + 1 - CirclePointCount);
					}
					else
					{
						indices[k++] = static_cast<std::uint16_t>(index + CirclePointCount);
						indices[k++] = static_cast<std::uint16_t>(index);
						indices[k++] = static_cast<std::uint16_t>(index + 1);
					}

					if (i < ringCount - 1)
					{
						indices[k++] = static_cast<std::uint16_t>(index - 1 + CirclePointCount);
						indices[k++] = static_cast<std::uint16_t>(index);
						indices[k++] =static_cast<std::uint16_t>(index + CirclePointCount);
					}
				}
			}

			assert(k == indexCount && "Index count does not match!");
			vertices = reinterpret_cast<std::uint8_t*>(data);
		}

		//Hemisphere

		//Shpere
		std::uint8_t *Sphere::vertices{ nullptr };
		std::uint16_t *Sphere::indices{ nullptr };

		Sphere::Sphere(float radius) : Hemisphere(radius)
		{
			InitVerticeAndIndice();
		}

		void Sphere::InitVerticeAndIndice()
		{
			if (vertices)
				return;
			{
				if (!vertices)
				{
					auto vbSize = Hemisphere::GetVertexBufferSize();
					auto ibSize = Hemisphere::GetIndexBufferSize();
					auto superVB = Hemisphere::GetVertices();
					auto superIB = Hemisphere::GetIndices();
					auto vertexCount = Hemisphere::GetVertexCount();
					auto indexCount = Hemisphere::GetIndexCount();
					vertices = new std::uint8_t[vbSize * 2];
					indices = new std::uint16_t[2 * indexCount];
					
					std::memcpy(vertices, superVB, vbSize);
					std::memcpy(vertices + vbSize, superVB, vbSize);

					std::memcpy(indices, superIB, ibSize);
					std::memcpy(reinterpret_cast<std::uint8_t*>(indices) + ibSize, superIB, ibSize);

					Vector3f *verts = reinterpret_cast<Vector3f*>(vertices + vbSize);
					for (std::size_t i = 0;i < 2 * vertexCount;++i)
					{
						verts[i].y = -verts[i].y;
					}

					for (std::size_t i = indexCount;i < 2 * indexCount;++i)
					{
						indices[i] += static_cast<std::uint16_t>(vertexCount);
						if (i % 3 == 2)
						{
							std::swap(indices[i], indices[i - 1]);
						}
					}

				}
			}
		}
		//Sphere

	}
}