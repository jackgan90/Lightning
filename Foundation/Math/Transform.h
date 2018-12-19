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
				Transform(): mPosition{0.0f, 0.0f, 0.0f} ,mRotation{0, 0, 0, 1} ,mScale{1.0f, 1.0f, 1.0f} ,mMatrixDirty(true) { }
				Transform(const Vector3f& pos, const Vector3f& scale, const Quaternionf& rot)
					:mPosition(pos), mScale(scale), mRotation(rot), mMatrixDirty(true){}

				Matrix4f LocalToGlobalMatrix4()
				{
					UpdateMatrix();
					return mMatrix;
				}
				Matrix4f GlobalToLocalMatrix4()
				{
					UpdateMatrix();
					return mInvMatrix;
				}

				void SetPosition(const Vector3f& position)
				{
					mPosition = position;
					mMatrixDirty = true;
				}
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
						Quaternionf quat;
						quat.FromAxisAndAngle(desiredUp, std::cos(fwdDot));
						mRotation = quat * mRotation;
						//LOG_INFO("TRIGGER, x, y, z, w : %f, %f, %f, %f", mRotation.x, mRotation.y, mRotation.z, mRotation.w);
					}

					mMatrixDirty = true;
				}
				Vector3f Forward()const { return mRotation * Vector3f::back(); }
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

					return Quaternionf::MakeRotation(Vector3f::back(), direction);
				}
			private:
				void UpdateMatrix()
				{
					if (!mMatrixDirty)
						return;
					mMatrixDirty = false;

					Matrix4f matTrans;
					matTrans.SetIdentity();
					matTrans.SetColumn(3, Vector4f{mPosition.x, mPosition.y, mPosition.z, 1.0f});

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
					matTrans.SetColumn(3, Vector4f{-mPosition.x, -mPosition.y, -mPosition.z, 1.0f});

					mInvMatrix = matScale * matRotation * matTrans;
				}
				Vector3f mPosition;
				Vector3f mScale;
				Quaternionf mRotation;
				bool mMatrixDirty;
				Matrix4f mMatrix;
				Matrix4f mInvMatrix;
			};
		}
	}
}
