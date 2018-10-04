#pragma once
#include "rendererexportdef.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "math/quaternion.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Quaternionf;
		class LIGHTNING_RENDER_API Transform
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
			void OrientTo(const Vector3f& direction, const Vector3f& up = Vector3f::up());
			Vector3f Yaw()const { return mRotation.RotateVector(Vector3f::back()); }
			Vector3f Pitch()const { return mRotation.RotateVector(Vector3f::up()); }
			Vector3f Roll()const { return mRotation.RotateVector(Vector3f::right()); }
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
