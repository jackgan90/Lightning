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

		Cube::Cube(float size) : 
		Primitive(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
		m_size(size)
		{
			assert(m_size > 0 && "The size of the cube must be greater than 0!");
		}

		Cube::~Cube()
		{
			if (m_vertices)
			{
				delete[] m_vertices;
			}
			if (m_indices)
			{
				delete[] m_indices;
			}
		}

	}
}