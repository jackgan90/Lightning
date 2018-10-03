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
			
			mMatrix.SetIdentity();
			mMatrix.SetColumn(3, Vector4f(mPosition));

			Matrix4f matRotation;
			mRotation.ToMatrix(matRotation);

			mMatrix *= matRotation;

			mMatrix.m[Matrix4f::CELL_INDEX(0, 0)] *= mScale.x;
			mMatrix.m[Matrix4f::CELL_INDEX(1, 1)] *= mScale.y;
			mMatrix.m[Matrix4f::CELL_INDEX(2, 2)] *= mScale.z;
		}

		void Transform::LookAt(const Vector3f& position, const Vector3f& up)
		{
			auto direction = position - mPosition;
			if (direction.IsZero())
				return;
			mRotation.OrientTo(direction, up);
			mMatrixDirty = true;
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

		Matrix4f Transform::GlobalToLocalMatrix4()
		{
			Transform transInverse = Inversed();
			return transInverse.ToMatrix4();
		}

		Transform Transform::Inversed()const
		{
			return Transform(-mPosition, mScale * 0.5f, mRotation.Inversed());
		}
	}
}
