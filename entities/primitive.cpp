#include <cassert>
#include "primitive.h"

namespace LightningGE
{
	namespace Entities
	{
		Primitive::Primitive(PrimitiveType topology) : m_primType(topology)
		{
		}

		Primitive::~Primitive()
		{

		}

		void Primitive::Draw(Render::Renderer& renderer)
		{
			renderer.Draw(m_geo, m_material, m_transform);
		}

		float Cube::verticeTemplate[24] = { 0.5f, 0.5f, -0.5f,  //right top front
											0.5f, 0.5f, 0.5f,	//right top back
											0.5f, -0.5f, -0.5f,	//right bottom front
											0.5f, -0.5f, 0.5f,	//right bottom back
											-0.5f, 0.5f, -0.5f,	//left top front
											-0.5f, 0.5f, 0.5f,	//left top back
											-0.5f, -0.5f, -0.5f,//left bottom front
											-0.5f, -0.5f, 0.5f};//left bottom back

		std::uint16_t Cube::s_indices[36] = 
		{
			//right face
			0, 2, 3, 0, 3, 1,
			//front face
			4, 6, 2, 4, 2, 0,
			//left face
			5, 7, 6, 5, 6, 4,
			//back face
			1, 3, 6, 1, 6, 5,
			//top face
			5, 4, 0, 5, 0, 1,
			//bottom face
			6, 7, 3, 6, 3, 2
		};
		Cube::Cube(float size) : 
		Primitive(Render::PrimitiveType::TRIANGLE_LIST),
			m_size(size) 
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
			m_geo = std::make_shared<Render::Geometry>();
			m_geo->vbs[0] = std::make_unique<Render::VertexBuffer>(comps);
			m_geo->ib = std::make_unique<Render::IndexBuffer>(Render::IndexType::UINT16);
			float* vertices = new float[sizeof(float) * 24];
			for (std::size_t i = 0;i < 24;++i)
			{
				vertices[i] = verticeTemplate[i] * size;
			}

			m_vertices = reinterpret_cast<std::uint8_t*>(vertices);
			m_geo->vbs[0]->SetBuffer(m_vertices, sizeof(float) * 24);
			m_geo->ib->SetBuffer(reinterpret_cast<std::uint8_t*>(&s_indices), sizeof(s_indices));
			std::fill(std::begin(m_geo->vbs_dirty), std::end(m_geo->vbs_dirty), false);
			m_geo->vbs_dirty[0] = true;
			m_geo->ib_dirty = true;
			m_geo->primType = Render::PrimitiveType::TRIANGLE_LIST;
			m_material = std::make_shared<Render::Material>();
			m_material->RequireSemantic(Render::RenderSemantics::WVP);
			m_transform = Render::Transform(Render::Vector3f({ 1.0f, 1.0f, 1.0f }), Render::Vector3f({ 2.0f, 2.0f, 2.0f }));
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