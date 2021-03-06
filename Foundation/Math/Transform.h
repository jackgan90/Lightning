#pragma once
#undef min
#undef max
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			class Transform
			{
			public:
				Transform(const Vector3f& pos, const Vector3f& scale, const Quaternionf& rot)
					: mPosition(pos)
					, mRotation(rot)
					, mScale(scale)
					, mMatrixDirty(true)
				{

				}

				Transform() : Transform(
					Vector3f{ 0.0f, 0.0f, 0.0f }, 
					Vector3f{ 1.0f, 1.0f, 1.0f }, 
					Quaternionf{0.0f, 0.0f, 0.0f, 1.0f})
				{
				}

				Matrix4f LocalToGlobalMatrix4()const
				{
					UpdateMatrix();
					return mMatrix;
				}
				Matrix4f GlobalToLocalMatrix4()const
				{
					UpdateMatrix();
					return mInverseMatrix;
				}

				void SetPosition(const Vector3f& position)
				{
					mPosition = position;
					mMatrixDirty = true;
				}

				Matrix4f GetMatrix()const{ UpdateMatrix(); return mMatrix; }
				Vector3f GetPosition()const { return mPosition; }

				void SetRotation(const Quaternionf& rotation)
				{
					assert(rotation.Length() >= 0.999 && rotation.Length() <= 1.0001);
					mRotation = rotation;
					mMatrixDirty = true;
				}
				Quaternionf GetRotation()const { return mRotation; }

				void SetScale(const Vector3f& scale)
				{
					mScale = scale;
					mMatrixDirty = true;
				}
				Vector3f GetScale()const { return mScale; }

				void LookAt(const Vector3f& position, const Vector3f& up = Vector3f::up())
				{
					auto direction = position - mPosition;
					if (direction.IsZero())
						return;
					OrientTo(direction, up);
				}

				void OrientTo(Vector3f direction, Vector3f up = Vector3f::up())
				{
					up.Normalize();
					direction.Normalize();

					auto left = direction.Cross(up);
					if (left.IsZero())
					{
						left = -Right();
					}
					auto desiredUp = left.Cross(direction);

					auto rot1 = Quaternionf::MakeRotation(Vector3f::forward(), direction);

					Vector3f newUp = rot1 * Vector3f::up();

					auto rot2 = Quaternionf::MakeRotation(newUp, desiredUp);

					mRotation = rot2 * rot1;

					auto fwdDot = Forward().Dot(direction);
					fwdDot = std::min(std::max(-1.0f, fwdDot), 1.0f);
					if (fwdDot < 0)
					{
						Quaternionf quat;
						quat.FromAxisAndAngle(desiredUp, std::acos(fwdDot));
						mRotation = quat * mRotation;
						//LOG_INFO("TRIGGER, x, y, z, w : %f, %f, %f, %f", mRotation.x, mRotation.y, mRotation.z, mRotation.w);
					}

					mMatrixDirty = true;
				}

				Vector3f LocalPointToGlobal(const Vector3f& point)const
				{
					auto globalPoint = Vector4f{ point.x, point.y, point.z, 1.f } * LocalToGlobalMatrix4();
					return Vector3f{ globalPoint.x, globalPoint.y, globalPoint.z };
				}

				Vector3f LocalDirectionToGlobal(const Vector3f& direction)const
				{
					auto globalDirection = Vector4f{ direction.x, direction.y, direction.z, 0.f } * LocalToGlobalMatrix4();
					return Vector3f{ globalDirection.x, globalDirection.y, globalDirection.z };
				}

				Vector3f GlobalPointToLocal(const Vector3f& point)const
				{
					auto localPoint = Vector4f{ point.x, point.y, point.z, 1.f } * GlobalToLocalMatrix4();
					return Vector3f{ localPoint.x, localPoint.y, localPoint.z };
				}

				Vector3f GlobalDirectionToLocal(const Vector3f& direction)const
				{
					auto localDirection = Vector4f{ direction.x, direction.y, direction.z, 0.f } * GlobalToLocalMatrix4();
					return Vector3f{ localDirection.x, localDirection.y, localDirection.z };
				}

				Vector3f Forward()const { return mRotation * Vector3f::forward(); }
				Vector3f Up()const { return mRotation * Vector3f::up(); }
				Vector3f Right()const { return mRotation * Vector3f::right(); }
				//ref : https://math.stackexchange.com/questions/44689/how-to-find-a-random-axis-or-unit-vector-in-3d
				static Quaternionf RandomRotation()
				{
					static std::random_device rd;
					static std::mt19937 mt(rd());
					static std::uniform_real_distribution<float> zDist(-1, 1);
					static std::uniform_real_distribution<float> thetaDist(0, float(2 * PI));
					auto z = zDist(mt);
					auto theta = thetaDist(mt);
					auto coef = std::sqrt(1 - z * z);

					auto direction = Vector3f{coef * std::cos(theta), coef * std::sin(theta), z};

					return Quaternionf::MakeRotation(Vector3f::forward(), direction);
				}
			private:
				void UpdateMatrix()const
				{
					if (!mMatrixDirty)
						return;
					mMatrixDirty = false;

					Matrix4f matTrans;
					matTrans.SetIdentity();
					matTrans.SetRow(3, Vector4f{mPosition.x, mPosition.y, mPosition.z, 1.0f});

					Matrix4f matRotation;
					mRotation.ToMatrix(matRotation);

					Matrix4f matScale;
					matScale.SetIdentity();
					matScale.SetCell(0, 0, mScale.x);
					matScale.SetCell(1, 1, mScale.y);
					matScale.SetCell(2, 2, mScale.z);

					mMatrix = matScale * matRotation * matTrans;

					matScale.SetIdentity();
					matScale.SetCell(0, 0, float(1.0 / mScale.x));
					matScale.SetCell(1, 1, float(1.0 / mScale.y));
					matScale.SetCell(2, 2, float(1.0 / mScale.z));

					mRotation.Inversed().ToMatrix(matRotation);

					matTrans.SetIdentity();
					matTrans.SetRow(3, Vector4f{-mPosition.x, -mPosition.y, -mPosition.z, 1.0f});

					mInverseMatrix = matTrans * matRotation * matScale;
				}
				Vector3f mPosition;
				Vector3f mScale;
				Quaternionf mRotation;
				mutable bool mMatrixDirty;
				mutable Matrix4f mMatrix;
				mutable Matrix4f mInverseMatrix;
			};
		}
	}
}
