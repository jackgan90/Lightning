#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include "primitive.h"

namespace Lightning
{
	namespace Scene
	{
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
			mRenderItem.transform = Render::Transform(Render::Vector3f({ 0.0f, 0.0f, 0.0f }), Render::Vector3f({ 1.0f, 1.0f, 1.0f }));
		}

		Cube::~Cube()
		{
		}
		//Cube

		//Cylinder
		Cylinder::Cylinder(float height, float radius) :mHeight(height), mRadius(radius)
		{
			//mVertices include 74 vector3s upper circle + lower circle + 2 center
			mVertices = new std::uint8_t[sizeof(float) * 3 * 74];

			for (int i = 0;i < 36;++i)
			{
			}
		}

		Cylinder::~Cylinder()
		{
		}
		//Cylinder

	}
}