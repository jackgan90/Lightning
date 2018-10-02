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
		using Foundation::Math::Vector3f;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Quaternionf;
		class LIGHTNING_RENDER_API Transform
		{
		public:
			Transform();
			Transform(const Vector3f& pos, const Vector3f& scale);
			Matrix4f GetTransformMatrix()const;
			void SetPosition(const Vector3f& position);
			Vector3f GetPosition()const { return mPosition; }
			void SetRotation(const Quaternionf& rotation);
			Quaternionf GetRotation()const { return mRotation; }
			void SetScale(const Vector3f& scale);
			Vector3f GetScale()const { return mScale; }
		private:
			void UpdateTransformMatrix();
			//TODO : add quaternion to represent rotation
			Vector3f mPosition;
			Vector3f mScale;
			Quaternionf mRotation;
			Matrix4f mTransform;
		};
	}
}
