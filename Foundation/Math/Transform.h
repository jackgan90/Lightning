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
			struct Transform
			{
				Vector3f position;
				Vector3f scale;
				Quaternionf rotation;
				Matrix4f matrix;
				Matrix4f inverseMatrix;
			};
			static_assert(std::is_pod<Transform>::value, "Transform is not a POD.");

			class Transformer
			{
			public:
				Transformer(): mMatrixDirty(true)
				{
					mTransform.position = Vector3f{ 0.0f, 0.0f, 0.0f };
					mTransform.rotation = Quaternionf{ 0.0f, 0.0f, 0.0f, 1.0f };
					mTransform.scale = Vector3f{ 1.0f, 1.0f, 1.0f };
				}
				Transformer(const Vector3f& pos, const Vector3f& scale, const Quaternionf& rot)
					:mMatrixDirty(true)
				{
					mTransform.position = pos;
					mTransform.scale = scale;
					mTransform.rotation = rot;
				}

				Matrix4f LocalToGlobalMatrix4()
				{
					UpdateMatrix();
					return mTransform.matrix;
				}
				Matrix4f GlobalToLocalMatrix4()
				{
					UpdateMatrix();
					return mTransform.inverseMatrix;
				}

				void SetPosition(const Vector3f& position)
				{
					mTransform.position = position;
					mMatrixDirty = true;
				}
				Vector3f GetPosition()const { return mTransform.position; }

				void SetRotation(const Quaternionf& rotation)
				{
					assert(rotation.Length() >= 0.999 && rotation.Length() <= 1.0001);
					mTransform.rotation = rotation;
					mMatrixDirty = true;
				}
				Quaternionf GetRotation()const { return mTransform.rotation; }

				void SetScale(const Vector3f& scale)
				{
					mTransform.scale = scale;
					mMatrixDirty = true;
				}
				Vector3f GetScale()const { return mTransform.scale; }

				void LookAt(const Vector3f& position, const Vector3f& up = Vector3f::up())
				{
					auto direction = position - mTransform.position;
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

					mTransform.rotation = rot2 * rot1;

					auto fwdDot = Forward().Dot(direction);
					fwdDot = std::min(std::max(-1.0f, fwdDot), 1.0f);
					if (fwdDot < 0)
					{
						Quaternionf quat;
						quat.FromAxisAndAngle(desiredUp, std::cos(fwdDot));
						mTransform.rotation = quat * mTransform.rotation;
						//LOG_INFO("TRIGGER, x, y, z, w : %f, %f, %f, %f", mRotation.x, mRotation.y, mRotation.z, mRotation.w);
					}

					mMatrixDirty = true;
				}
				Transform GetTransform() { UpdateMatrix(); return mTransform; }
				Vector3f Forward()const { return mTransform.rotation * Vector3f::back(); }
				Vector3f Up()const { return mTransform.rotation * Vector3f::up(); }
				Vector3f Right()const { return mTransform.rotation * Vector3f::right(); }
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
					matTrans.SetColumn(3, Vector4f{mTransform.position.x, mTransform.position.y, mTransform.position.z, 1.0f});

					Matrix4f matRotation;
					mTransform.rotation.ToMatrix(matRotation);

					Matrix4f matScale;
					matScale.SetIdentity();
					matScale.SetCell(0, 0, mTransform.scale.x);
					matScale.SetCell(1, 1, mTransform.scale.y);
					matScale.SetCell(2, 2, mTransform.scale.z);

					mTransform.matrix = matTrans * matRotation * matScale;

					matScale.SetIdentity();
					matScale.SetCell(0, 0, float(1.0 / mTransform.scale.x));
					matScale.SetCell(1, 1, float(1.0 / mTransform.scale.y));
					matScale.SetCell(2, 2, float(1.0 / mTransform.scale.z));

					mTransform.rotation.Inversed().ToMatrix(matRotation);

					matTrans.SetIdentity();
					matTrans.SetColumn(3, Vector4f{-mTransform.position.x, -mTransform.position.y, -mTransform.position.z, 1.0f});

					mTransform.inverseMatrix = matScale * matRotation * matTrans;
				}
				bool mMatrixDirty;
				Transform mTransform;
			};
		}
	}
}
