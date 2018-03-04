#include "transform.h"

namespace LightningGE
{
	namespace Render
	{
		Transform::Transform():m_pos{0.0f, 0.0f, 0.0f}, m_scale{1.0f, 1.0f, 1.0f}
		{
			UpdateTransformMatrix();
		}

		Transform::Transform(const Vector3f& pos, const Vector3f& scale) :
			m_pos(pos), m_scale(scale)
		{
			UpdateTransformMatrix();
		}

		void Transform::UpdateTransformMatrix()
		{
			Matrix4x4f scaleMatrix;
			scaleMatrix(0, 0) = m_scale[0];
			scaleMatrix(1, 1) = m_scale[1];
			scaleMatrix(2, 2) = m_scale[2];
			
			Matrix4x4f translationMatrix;
			Vector4f column(m_pos);
			column[3] = 1.0f;
			translationMatrix.SetColumns(3, column);
			
			m_transform = translationMatrix * scaleMatrix;
		}

		Matrix4x4f Transform::GetTransformMatrix()const
		{
			return m_transform;
		}
	}
}
