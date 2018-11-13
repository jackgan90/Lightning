#include <cassert>
#include <algorithm>
#include <random>
#include "Logger.h"
#include "transform.h"
#undef min
#undef max

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
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
				if (!mMatrixDirty)
					return;
				mMatrixDirty = false;

				Matrix4f matTrans;
				matTrans.SetIdentity();
				matTrans.SetColumn(3, Vector4f(mPosition));

				Matrix4f matRotation;
				mRotation.ToMatrix(matRotation);

				Matrix4f matScale;
				matScale.SetIdentity();
				matScale.SetCell(0, 0, mScale.x);
				matScale.SetCell(1, 1, mScale.y);
				matScale.SetCell(2, 2, mScale.z);

				mMatrix = matTrans * matRotation * matScale;

				matScale.SetIdentity();
				matScale.SetCell(0, 0, float(1.0 / mScale.x));
				matScale.SetCell(1, 1, float(1.0 / mScale.y));
				matScale.SetCell(2, 2, float(1.0 / mScale.z));

				mRotation.Inversed().ToMatrix(matRotation);

				matTrans.SetIdentity();
				matTrans.SetColumn(3, Vector4f(-mPosition));

				mInvMatrix = matScale * matRotation * matTrans;
			}

			void Transform::LookAt(const Vector3f& position, const Vector3f& up)
			{
				auto direction = position - mPosition;
				if (direction.IsZero())
					return;
				OrientTo(direction, up);
			}

			void Transform::OrientTo(Vector3f direction, Vector3f up)
			{
				up.Normalize();
				direction.Normalize();

				auto right = direction.Cross(up);
				if (right.IsZero())
				{
					right = Right();
				}
				auto desiredUp = right.Cross(direction);

				auto rot1 = Quaternionf::MakeRotation(Vector3f::back(), direction);

				Vector3f newUp = rot1 * Vector3f::up();

				auto rot2 = Quaternionf::MakeRotation(newUp, desiredUp);

				mRotation = rot2 * rot1;

				auto fwdDot = Forward().Dot(direction);
				fwdDot = std::min(std::max(-1.0f, fwdDot), 1.0f);
				if (fwdDot < 0)
				{
					mRotation = Quaternionf(desiredUp, std::acos(fwdDot)) * mRotation;
					//LOG_INFO("TRIGGER, x, y, z, w : %f, %f, %f, %f", mRotation.x, mRotation.y, mRotation.z, mRotation.w);
				}

				mMatrixDirty = true;
			}

			Matrix4f Transform::LocalToGlobalMatrix4()
			{
				UpdateMatrix();
				return mMatrix;
			}

			Matrix4f Transform::GlobalToLocalMatrix4()
			{
				UpdateMatrix();
				return mInvMatrix;
			}

			//ref : https://math.stackexchange.com/questions/44689/how-to-find-a-random-axis-or-unit-vector-in-3d
			Quaternionf Transform::RandomRotation()
			{
				static std::random_device rd;
				static std::mt19937 mt(rd());
				static std::uniform_real_distribution<float> zDist(-1, 1);
				static std::uniform_real_distribution<float> thetaDist(0, float(2 * PI));
				auto z = zDist(mt);
				auto theta = thetaDist(mt);
				auto coef = std::sqrt(1 - z * z);

				auto direction = Vector3f(coef * std::cos(theta), coef * std::sin(theta), z);

				return Quaternionf::MakeRotation(Vector3f::back(), direction);
			}
		}

	}
}
