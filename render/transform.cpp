#include "transform.h"

namespace Lightning
{
	namespace Render
	{
		Transform::Transform():mPosition{0.0f, 0.0f, 0.0f}, mScale{1.0f, 1.0f, 1.0f}
		{
			UpdateTransformMatrix();
		}

		Transform::Transform(const Vector3f& pos, const Vector3f& scale) :
			mPosition(pos), mScale(scale)
		{
			UpdateTransformMatrix();
		}

		void Transform::UpdateTransformMatrix()
		{
			Matrix4x4f scaleMatrix;
			scaleMatrix(0, 0) = mScale[0];
			scaleMatrix(1, 1) = mScale[1];
			scaleMatrix(2, 2) = mScale[2];
			
			Matrix4x4f translationMatrix;
			Vector4f column(mPosition);
			column[3] = 1.0f;
			translationMatrix.SetColumns(3, column);
			
			mTransform = translationMatrix * scaleMatrix;
		}

		Matrix4x4f Transform::GetTransformMatrix()const
		{
			return mTransform;
		}
	}
}
