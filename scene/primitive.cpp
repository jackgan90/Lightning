#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include "math/vector.h"
#include "primitive.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::DegreesToRadians;
		Primitive::Primitive():mVertices(nullptr),mIndices(nullptr)
		{
		}

		Primitive::~Primitive()
		{
			if(mVertices)
				delete[] mVertices;
			if(mIndices)
				delete[] mIndices;
		}

		void Primitive::Draw(Render::Renderer& renderer, const SceneRenderData& sceneRenderData)
		{
			if (sceneRenderData.camera)
			{
				mRenderItem.viewMatrix = sceneRenderData.camera->GetViewMatrix();
				mRenderItem.projectionMatrix = sceneRenderData.camera->GetProjectionMatrix();
			}
			renderer.Draw(mRenderItem);
		}
		
		//Cube
		float Cube::sVerticeTemplate[] = { 0.5f, 0.5f, 0.5f,  //right top front v0
											0.5f, 0.5f, -0.5f,	//right top back v1
											0.5f, -0.5f, 0.5f,	//right bottom front v2
											0.5f, -0.5f, -0.5f,	//right bottom back v3
											-0.5f, 0.5f, 0.5f,	//left top front v4
											-0.5f, 0.5f, -0.5f,	//left top back v5
											-0.5f, -0.5f, 0.5f,  //left bottom front v6
											-0.5f, -0.5f, -0.5f };//left bottom back v7

		std::uint16_t Cube::sIndices[] = 
		{
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
		Cube::Cube(float size) : mSize(size) 
		{
			assert(mSize > 0 && "The size of the cube must be greater than 0!");
			Render::VertexComponent comp;
			comp.format = Render::RenderFormat::R32G32B32_FLOAT;
			comp.instanceStepRate = 1;
			comp.isInstance = false;
			comp.offset = 0;
			comp.semanticIndex = 0;
			comp.semanticItem = { Render::RenderSemantics::POSITION, "POSITION" };
			std::vector<Render::VertexComponent> comps;
			comps.push_back(comp);
			mRenderItem.geometry = std::make_shared<Render::Geometry>();
			auto pDevice = Renderer::Instance()->GetDevice();
			mRenderItem.geometry->vbs[0] = pDevice->CreateVertexBuffer(sizeof(sVerticeTemplate), comps);
			mRenderItem.geometry->ib = pDevice->CreateIndexBuffer(sizeof(sIndices), Render::IndexType::UINT16);
			
			
			float* vertices = new float[sizeof(sVerticeTemplate) / sizeof(float)];
			for (std::size_t i = 0;i < sizeof(sVerticeTemplate) / sizeof(float);++i)
			{
				vertices[i] = sVerticeTemplate[i] * size;
			}

			mVertices = reinterpret_cast<std::uint8_t*>(vertices);
			
			mRenderItem.geometry->vbs[0]->SetBuffer(mVertices, sizeof(sVerticeTemplate));
			mRenderItem.geometry->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(&sIndices), sizeof(sIndices));
			std::fill(std::begin(mRenderItem.geometry->vbs_dirty), std::end(mRenderItem.geometry->vbs_dirty), false);
			mRenderItem.geometry->vbs_dirty[0] = true;
			mRenderItem.geometry->ib_dirty = true;
			mRenderItem.geometry->primType = Render::PrimitiveType::TRIANGLE_LIST;
			
			mRenderItem.material = std::make_shared<Render::Material>();
			mRenderItem.material->RequireSemantic(Render::RenderSemantics::WVP);
			auto device = Renderer::Instance()->GetDevice();
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::VERTEX));
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::FRAGMENT));
			mRenderItem.transform = Render::Transform(Render::Vector3f({ 1.0f, 0.0f, 0.0f }), Render::Vector3f({ 1.0f, 1.0f, 1.0f }));
		}

		Cube::~Cube()
		{
		}
		//Cube

		//Cylinder
		Cylinder::Cylinder(float height, float radius) :mHeight(height), mRadius(radius)
		{
			CreateVerticesAndIndices();
			Render::VertexComponent comp;
			comp.format = Render::RenderFormat::R32G32B32_FLOAT;
			comp.instanceStepRate = 1;
			comp.isInstance = false;
			comp.offset = 0;
			comp.semanticIndex = 0;
			comp.semanticItem = { Render::RenderSemantics::POSITION, "POSITION" };
			std::vector<Render::VertexComponent> comps;
			comps.push_back(comp);
			mRenderItem.geometry = std::make_shared<Render::Geometry>();
			auto pDevice = Renderer::Instance()->GetDevice();
			mRenderItem.geometry->vbs[0] = pDevice->CreateVertexBuffer(VertexCount * sizeof(Vector3f), comps);
			mRenderItem.geometry->ib = pDevice->CreateIndexBuffer(IndexCount * sizeof(std::uint16_t), Render::IndexType::UINT16);
			
			mRenderItem.geometry->vbs[0]->SetBuffer(mVertices, VertexCount * sizeof(Vector3f));
			mRenderItem.geometry->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(mIndices), IndexCount * sizeof(std::uint16_t));
			std::fill(std::begin(mRenderItem.geometry->vbs_dirty), std::end(mRenderItem.geometry->vbs_dirty), false);
			mRenderItem.geometry->vbs_dirty[0] = true;
			mRenderItem.geometry->ib_dirty = true;
			mRenderItem.geometry->primType = Render::PrimitiveType::TRIANGLE_LIST;
			
			mRenderItem.material = std::make_shared<Render::Material>();
			mRenderItem.material->RequireSemantic(Render::RenderSemantics::WVP);
			auto device = Renderer::Instance()->GetDevice();
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::VERTEX));
			mRenderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::FRAGMENT));
			mRenderItem.transform = Render::Transform(Render::Vector3f({ 0.0f, 0.0f, 0.0f }), Render::Vector3f({ 1.0f, 1.0f, 1.0f }));
		}

		void Cylinder::CreateVerticesAndIndices()
		{
			static const auto CirclePointCount = 360 / AngularUnit;
			Vector3f *vertices = new Vector3f[VertexCount];
			mIndices = new std::uint16_t[IndexCount];
			std::memset(mIndices, 0, sizeof(std::uint16_t) * IndexCount);
			
			Vector3f *topCenter = &vertices[0];
			topCenter->x = 0;
			topCenter->y = static_cast<float>(mHeight / 2.0);
			topCenter->z = 0;

			Vector3f *bottomCenter = &vertices[VertexCount - 1];
			bottomCenter->x = 0;
			bottomCenter->y = static_cast<float>(-mHeight / 2.0);
			bottomCenter->z = 0;
			int j = 0;
			
			for (int i = 0;i < CirclePointCount;++i)
			{
				//upper circle
				auto *vt = &vertices[i + 1];
				auto radians = DegreesToRadians(i * AngularUnit);
				*vt = *topCenter + mRadius * (Vector3f::right * std::cos(radians) + Vector3f::back * std::sin(radians));

				auto *vb = &vertices[i + 1 + CirclePointCount];
				*vb = *vt + Vector3f::down * mHeight;

				auto isLast = i == CirclePointCount - 1;
				
				//top triangle
				mIndices[j++] = 0;
				mIndices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
					mIndices[j++] = static_cast<std::uint16_t>(i + 2);
				else
					mIndices[j++] = static_cast<std::uint16_t>(2);
					

				//bottom triangle
				mIndices[j++] = static_cast<std::uint16_t>(VertexCount - 1);
				mIndices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
				if (!isLast)
					mIndices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
				else
					mIndices[j++] = static_cast<std::uint16_t>(2 + CirclePointCount);

				//side triangle 0
				mIndices[j++] = static_cast<std::uint16_t>(i + 1);
				if (!isLast)
				{
					mIndices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
					mIndices[j++] = static_cast<std::uint16_t>(i + 2);
				}
				else
				{
					mIndices[j++] = static_cast<std::uint16_t>(2 + CirclePointCount);
					mIndices[j++] = static_cast<std::uint16_t>(2);
				}

				//side triangle 1
				mIndices[j++] = static_cast<std::uint16_t>(i + 1);
				mIndices[j++] = static_cast<std::uint16_t>(i + 1 + CirclePointCount);
				if(!isLast)
					mIndices[j++] = static_cast<std::uint16_t>(i + 2 + CirclePointCount);
				else
					mIndices[j++] = static_cast<std::uint16_t>(2 + CirclePointCount);
			}
			mVertices = reinterpret_cast<std::uint8_t*>(vertices);
		}


		Cylinder::~Cylinder()
		{
		}
		//Cylinder

	}
}