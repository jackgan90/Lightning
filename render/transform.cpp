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
			OrientTo(direction, up);
		}

		void Transform::OrientTo(const Vector3f& direction, const Vector3f& up)
		{
			assert(!direction.IsZero());
			mRotation.OrientTo(direction.Normalized(), up);
			mMatrixDirty = true;
		}

		Matrix4f Transform::LocalToGlobalMatrix4()
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
			Matrix4f mat;
			mat.SetIdentity();
			mat.SetCell(0, 0, float(1.0 / mScale.x));
			mat.SetCell(1, 1, float(1.0 / mScale.y));
			mat.SetCell(2, 2, float(1.0 / mScale.z));

			Matrix4f matTrans;
			matTrans.SetIdentity();
			matTrans.SetColumn(3, Vector4f(-mPosition));

			Matrix4f matRotation;
			mRotation.Inversed().ToMatrix(matRotation);
			return mat * matRotation * matTrans;
		}
	}
}
