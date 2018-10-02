#include "transform.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector4f;
		Transform::Transform():
			mPosition(0.0f, 0.0f, 0.0f)
			,mRotation(0, 0, 0, 1)
			,mScale(1.0f, 1.0f, 1.0f)
		{
			UpdateTransformMatrix();
		}

		Transform::Transform(const Vector3f& pos, const Vector3f& scale) :
			mPosition(pos), mScale(scale)
		{
			UpdateTransformMatrix();
		}

		void Transform::SetPosition(const Vector3f& position)
		{
			mPosition = position;
			UpdateTransformMatrix();
		}

		void Transform::SetRotation(const Quaternionf& rotation)
		{
			mRotation = rotation;
			UpdateTransformMatrix();
		}

		void Transform::SetScale(const Vector3f& scale)
		{
			mScale = scale;
			UpdateTransformMatrix();
		}

		void Transform::UpdateTransformMatrix()
		{
			Matrix4f matScale;
			matScale.SetZero();
			matScale.m[0] = mScale[0];
			matScale.m[5] = mScale[1];
			matScale.m[10] = mScale[2];
			matScale.m[15] = 1.0f;

			Matrix4f matRotation;
			mRotation.ToMatrix(matRotation);
			
			Matrix4f matTranslation;
			matTranslation.SetIdentity();
			Vector4f column(mPosition);
			matTranslation.SetColumn(3, column);
			
			mTransform = matTranslation * matRotation * matScale;
		}

		Matrix4f Transform::GetTransformMatrix()const
		{
			return mTransform;
		}
	}
}
