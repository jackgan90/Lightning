#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "container.h"
#include "math/vector.h"
#include "primitive.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		using Foundation::Math::DegreesToRadians;
		using Foundation::container;
		using Render::ShaderArgument;
		Primitive::Primitive():mFirstDraw(true),mWorldPosition(0, 0, 0)
			,mColor(0, 0, 0, 255)
		{
		}

		Primitive::~Primitive()
		{

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

		void Primitive::Draw(Render::Renderer& renderer, const SceneRenderData& sceneRenderData)
		{
			if (mFirstDraw)
			{
				mFirstDraw = false;
				UpdateRenderItem();
			}
			if (sceneRenderData.camera)
			{
				mRenderItem.viewMatrix = sceneRenderData.camera->GetViewMatrix();
				mRenderItem.projectionMatrix = sceneRenderData.camera->GetProjectionMatrix();
			}
			renderer.Draw(mRenderItem);
		}

		void Primitive::UpdateRenderItem()
		{
			Render::VertexComponent comp;
			comp.format = Render::RenderFormat::R32G32B32_FLOAT;
			comp.instanceStepRate = 1;
			comp.isInstance = false;
			comp.offset = 0;
			comp.semanticIndex = 0;
			comp.semanticItem = { Render::RenderSemantics::POSITION, "POSITION" };
			container::vector<Render::VertexComponent> comps;
			comps.push_back(comp);
			mRenderItem.geometry = std::make_shared<Render::Geometry>();
			auto pDevice = Renderer::Instance()->GetDevice();
			auto vbSize = GetVertexBufferSize();
			auto ibSize = GetIndexBufferSize();
			mRenderItem.geometry->vbs[0] = pDevice->CreateVertexBuffer(vbSize, comps);
			mRenderItem.geometry->ib = pDevice->CreateIndexBuffer(ibSize, Render::IndexType::UINT16);
			
			
			auto vertices = GetVertices();

			auto indices = GetIndices();

			mRenderItem.geometry->vbs[0]->SetBuffer(vertices, vbSize);
			mRenderItem.geometry->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(indices), ibSize);
			std::fill(std::begin(mRenderItem.geometry->vbs_dirty), std::end(mRenderItem.geometry->vbs_dirty), false);
			mRenderItem.geometry->vbs_dirty[0] = true;
			mRenderItem.geometry->ib_dirty = true;
			mRenderItem.geometry->primType = Render::PrimitiveType::TRIANGLE_LIST;
			
			mRenderItem.material = std::make_shared<Render::Material>();
			mRenderItem.material->RequireSemantic(Render::RenderSemantics::WVP);
			auto device = Renderer::Instance()->GetDevice();
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::VERTEX));
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::FRAGMENT));
			float a, r, g, b;
			GetColor(a, r, g, b);
			mRenderItem.material->AddArgument(Render::ShaderType::FRAGMENT, ShaderArgument("color", Vector4f(r, g, b, a)));
			mRenderItem.material->EnableBlend(mColor.a != 0xff);
			mRenderItem.transform = Render::Transform(mWorldPosition, GetScale());
		}

		
		//Cube
		Cube::CubeDataSource::CubeDataSource()
		{
			auto pVecs = new Vector3f[8];
			pVecs[0] = { 0.5f, 0.5f, 0.5f };	//right top front v0
			pVecs[1] = { 0.5f, 0.5f, -0.5f };	//right top back v1
			pVecs[2] = { 0.5f, -0.5f, 0.5f };	//right bottom front v2
			pVecs[3] = { 0.5f, -0.5f, -0.5f };	//right bottom back v3
			pVecs[4] = { -0.5f, 0.5f, 0.5f };	//left top front v4
			pVecs[5] = { -0.5f, 0.5f, -0.5f };	//left top back v5
			pVecs[6] = { -0.5f, -0.5f, 0.5f };	//left bottom front v6
			pVecs[7] = { -0.5f, -0.5f, -0.5f };	//left bottom back v7
			vertices = reinterpret_cast<float*>(pVecs);
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
		//Cube

		//Cylinder
		Cylinder::CylinderDataSource::CylinderDataSource()
		{
			static const auto CirclePointCount = 360 / AngularUnit;
			auto vertexCount = GetVertexCount();
			auto indexCount = GetIndexCount();
			Vector3f *positions = new Vector3f[vertexCount];
			indices = new std::uint16_t[indexCount];
			std::memset(indices, 0, sizeof(std::uint16_t) * indexCount);
			
			Vector3f& topCenter = positions[0];
			topCenter.x = 0;
			topCenter.y = static_cast<float>(1.0 / 2.0);
			topCenter.z = 0;

			Vector3f& bottomCenter = positions[vertexCount - 1];
			bottomCenter.x = 0;
			bottomCenter.y = static_cast<float>(-1.0 / 2.0);
			bottomCenter.z = 0;
			int j = 0;
			
			for (int i = 0;i < CirclePointCount;++i)
			{
				//upper circle
				auto radians = DegreesToRadians(i * AngularUnit);
				positions[i + 1] = topCenter + 0.5f * (Vector3f::right() * std::cos(radians) + Vector3f::back() * std::sin(radians));

				auto *vb = &positions[i + 1 + CirclePointCount];
				positions[i + 1 + CirclePointCount] = positions[i+1] + Vector3f::down() * 1.0f;

				auto isLast = i == CirclePointCount - 1;
				
				//top triangle
				indices[j++] = 0;
				indices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
					indices[j++] = static_cast<std::uint16_t>(i + 2);
				else
					indices[j++] = static_cast<std::uint16_t>(1);
					

				//bottom triangle
				indices[j++] = static_cast<std::uint16_t>(vertexCount - 1);
				indices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
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
			vertices = reinterpret_cast<float*>(positions);
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
			Vector3f *positions = new Vector3f[vertexCount];
			indices = new std::uint16_t[indexCount];
			std::memset(indices, 0, sizeof(std::uint16_t) * indexCount);
			positions[vertexCount - 1].x = 0;
			positions[vertexCount - 1].y = 0.5;
			positions[vertexCount - 1].z = 0;
			auto ringCount = 90 % AngularUnit == 0 ? 90 / AngularUnit : 90 / AngularUnit + 1;
			std::size_t k = 0;
			for (std::size_t i = 0;i < ringCount;++i)
			{
				auto y_radians = DegreesToRadians(i * AngularUnit);
				auto y = 0.5f * std::sin(y_radians);
				auto radius_at_y = 0.5f * std::cos(y_radians);
				for (std::size_t j = 0;j < CirclePointCount;++j)
				{
					auto xz_radians = DegreesToRadians(j * AngularUnit);
					auto index = i * CirclePointCount + j;
					positions[index] = y * Vector3f::up() + radius_at_y * 
						(Vector3f::right() * std::cos(xz_radians) + Vector3f::back() * std::sin(xz_radians));
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
			vertices = reinterpret_cast<float*>(positions);
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

					Vector3f *positions = reinterpret_cast<Vector3f*>(vertices + vbSize);
					for (std::size_t i = 0;i < vertexCount;++i)
					{
						positions[i].y = -positions[i].y;
					}

					for (std::size_t i = indexCount;i < 2 * indexCount;++i)
					{
						indices[i] += static_cast<std::uint16_t>(vertexCount);
					}

				}
			}
		}
		//Sphere

	}
}