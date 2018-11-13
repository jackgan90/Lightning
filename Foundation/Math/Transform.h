#pragma once
#include "foundationexportdef.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			class LIGHTNING_FOUNDATION_API Transform
			{
			public:
				Transform();
				Transform(const Vector3f& pos, const Vector3f& scale, const Quaternionf& rot);
				Matrix4f LocalToGlobalMatrix4();
				Matrix4f GlobalToLocalMatrix4();
				void SetPosition(const Vector3f& position);
				Vector3f GetPosition()const { return mPosition; }
				void SetRotation(const Quaternionf& rotation);
				Quaternionf GetRotation()const { return mRotation; }
				void SetScale(const Vector3f& scale);
				Vector3f GetScale()const { return mScale; }
				void LookAt(const Vector3f& position, const Vector3f& up = Vector3f::up());
				void OrientTo(Vector3f direction, Vector3f up = Vector3f::up());
				Vector3f Forward()const { return mRotation * Vector3f::back(); }
				Vector3f Up()const { return mRotation * Vector3f::up(); }
				Vector3f Right()const { return mRotation * Vector3f::right(); }
				static Quaternionf RandomRotation();
			private:
				void UpdateMatrix();
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
