#include <cassert>
#include "primitive.h"

namespace LightningGE
{
	namespace Entities
	{
		Primitive::Primitive(PrimitiveTopology topology) : m_topology(topology)
		{

		}

		Primitive::~Primitive()
		{

		}

		void Primitive::Draw(Render::Renderer& renderer)
		{

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
		Primitive(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
		m_size(size)
		{
			assert(m_size > 0 && "The size of the cube must be greater than 0!");
			float* vertices = new float[sizeof(float) * 24];
			for (std::size_t i = 0;i < 24;++i)
			{
				vertices[i] = verticeTemplate[i] * size;
			}

			m_vertices = reinterpret_cast<std::uint8_t*>(vertices);
			m_vb->SetBuffer(m_vertices, sizeof(float) * 24);
			m_ib->SetBuffer(reinterpret_cast<std::uint8_t*>(&s_indices), sizeof(s_indices));
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