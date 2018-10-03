#include <cassert>
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
			,mMatrixDirty(true)
		{
		}

		Transform::Transform(const Vector3f& pos, const Vector3f& scale, const Quaternionf& rot) :
			mPosition(pos), mScale(scale), mRotation(rot), mMatrixDirty(true)
		{
		}

		void Transform::SetPosition(const Vector3f& position)
		{
			mPosition = position;
			mMatrixDirty = true;
		}

		void Transform::SetRotation(const Quaternionf& rotation)
		{
			assert(rotation.Length() >= 0.999 && rotation.Length() <= 1.0001);
			mRotation = rotation;
			mMatrixDirty = true;
		}

		void Transform::SetScale(const Vector3f& scale)
		{
			mScale = scale;
			mMatrixDirty = true;
		}

		void Transform::UpdateMatrix()
		{
			Matrix4f matScale;
			matScale.SetZero();
			matScale.SetCell(0, 0, mScale[0]);
			matScale.SetCell(1, 1, mScale[1]);
			matScale.SetCell(2, 2, mScale[2]);
			matScale.SetCell(3, 3, 1);

			Matrix4f matRotation;
			mRotation.ToMatrix(matRotation);
			
			Matrix4f matTranslation;
			matTranslation.SetIdentity();
			Vector4f column(mPosition);
			matTranslation.SetColumn(3, column);
			
			mMatrix = matTranslation * matRotation * matScale;
		}

		void Transform::LookAt(const Vector3f& position, const Vector3f& up)
		{

		}

		Matrix4f Transform::ToMatrix4()
		{
			if (mMatrixDirty)
			{
				UpdateMatrix();
				mMatrixDirty = false;
			}
			return mMatrix;
		}
	}
}
