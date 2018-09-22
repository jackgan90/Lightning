#pragma once
#include "rendererexportdef.h"
#include "math/matrix.h"
#include "math/vector.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Vector3f;
		using Foundation::Vector3f;
		using Foundation::Matrix4f;
		class LIGHTNING_RENDER_API Transform
		{
		public:
			Transform();
			Transform(const Vector3f& pos, const Vector3f& scale);
			Matrix4f GetTransformMatrix()const;
		private:
			void UpdateTransformMatrix();
			//TODO : add quaternion to represent rotation
			Vector3f mPosition;
			Vector3f mScale;
			Matrix4f mTransform;
		};
	}
}
