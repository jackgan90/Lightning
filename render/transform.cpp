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
			Matrix4f scaleMatrix;
			scaleMatrix.SetZero();
			scaleMatrix.m[0] = mScale[0];
			scaleMatrix.m[5] = mScale[1];
			scaleMatrix.m[10] = mScale[2];
			scaleMatrix.m[15] = 1.0f;
			
			Matrix4f translationMatrix;
			translationMatrix.SetIdentity();
			Vector4f column(mPosition);
			translationMatrix.SetColumn(column, 3);
			
			mTransform = translationMatrix * scaleMatrix;
		}

		Matrix4f Transform::GetTransformMatrix()const
		{
			return mTransform;
		}
	}
}
