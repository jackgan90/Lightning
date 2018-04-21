#include <cassert>
#include "primitive.h"

namespace LightningGE
{
	namespace Scene
	{
		Primitive::Primitive()
		{
		}

		Primitive::~Primitive()
		{

		}

		void Primitive::Draw(Render::Renderer& renderer, const SceneRenderData& sceneRenderData)
		{
			if (sceneRenderData.camera)
			{
				m_renderItem.viewMatrix = sceneRenderData.camera->GetViewMatrix();
				m_renderItem.projectionMatrix = sceneRenderData.camera->GetProjectionMatrix();
			}
			renderer.Draw(m_renderItem);
		}

		float Cube::verticeTemplate[] = { 0.5f, 0.5f, 0.5f,  //right top front v0
											0.5f, 0.5f, -0.5f,	//right top back v1
											0.5f, -0.5f, 0.5f,	//right bottom front v2
											0.5f, -0.5f, -0.5f,	//right bottom back v3
											-0.5f, 0.5f, 0.5f,	//left top front v4
											-0.5f, 0.5f, -0.5f,	//left top back v5
											-0.5f, -0.5f, 0.5f,  //left bottom front v6
											-0.5f, -0.5f, -0.5f };//left bottom back v7

		std::uint16_t Cube::s_indices[] = 
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
		Cube::Cube(float size) : m_size(size) 
		{
			assert(m_size > 0 && "The size of the cube must be greater than 0!");
			Render::VertexComponent comp;
			comp.format = Render::RenderFormat::R32G32B32_FLOAT;
			comp.instanceStepRate = 1;
			comp.isInstance = false;
			comp.offset = 0;
			comp.semanticIndex = 0;
			comp.semanticItem = { Render::RenderSemantics::POSITION, "POSITION" };
			std::vector<Render::VertexComponent> comps;
			comps.push_back(comp);
			m_renderItem.geometry = std::make_shared<Render::Geometry>();
			auto pDevice = Renderer::Instance()->GetDevice();
			//m_renderItem.geometry->vbs[0] = pDevice->CreateVertexBuffer(9 * sizeof(float), comps);
			//m_renderItem.geometry->ib = pDevice->CreateIndexBuffer(3 * sizeof(std::uint16_t), Render::IndexType::UINT16);
			m_renderItem.geometry->vbs[0] = pDevice->CreateVertexBuffer(sizeof(verticeTemplate), comps);
			m_renderItem.geometry->ib = pDevice->CreateIndexBuffer(sizeof(s_indices), Render::IndexType::UINT16);
			
			/*
			float* vertices = new float[9];
			vertices[0] = 0.0f;
			vertices[1] = 0.5f;
			vertices[2] = 0.5f;
			vertices[3] = 0.5f;
			vertices[4] = -0.5f;
			vertices[5] = 0.5f;
			vertices[6] = -0.5f;
			vertices[7] = -0.5f;
			vertices[8] = 0.5f;
			std::uint16_t* indices = new std::uint16_t[3];
			indices[0] = 0;
			indices[1] = 2;
			indices[2] = 1;
			m_vertices = reinterpret_cast<std::uint8_t*>(vertices);
			*/
			
			float* vertices = new float[sizeof(verticeTemplate) / sizeof(float)];
			for (std::size_t i = 0;i < sizeof(verticeTemplate) / sizeof(float);++i)
			{
				vertices[i] = verticeTemplate[i] * size;
			}

			m_vertices = reinterpret_cast<std::uint8_t*>(vertices);
			
			m_renderItem.geometry->vbs[0]->SetBuffer(m_vertices, sizeof(verticeTemplate));
			m_renderItem.geometry->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(&s_indices), sizeof(s_indices));
			//m_renderItem.geometry->vbs[0]->SetBuffer(m_vertices, 9 * sizeof(float));
			//m_renderItem.geometry->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(indices), 3 * sizeof(std::uint16_t));
			std::fill(std::begin(m_renderItem.geometry->vbs_dirty), std::end(m_renderItem.geometry->vbs_dirty), false);
			m_renderItem.geometry->vbs_dirty[0] = true;
			m_renderItem.geometry->ib_dirty = true;
			m_renderItem.geometry->primType = Render::PrimitiveType::TRIANGLE_LIST;
			
			m_renderItem.material = std::make_shared<Render::Material>();
			m_renderItem.material->RequireSemantic(Render::RenderSemantics::WVP);
			auto device = Renderer::Instance()->GetDevice();
			m_renderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::VERTEX));
			m_renderItem.material->SetShader(device->GetDefaultShader(Render::ShaderType::FRAGMENT));
			m_renderItem.transform = Render::Transform(Render::Vector3f({ 0.0f, 0.0f, 0.0f }), Render::Vector3f({ 1.0f, 1.0f, 1.0f }));
		}

		Cube::~Cube()
		{
			if (m_vertices)
			{
				delete[] m_vertices;
			}
		}

	}
}