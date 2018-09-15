#pragma once
#include "rendererexportdef.h"
#include "types/vector.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API Transform
		{
		public:
			Transform();
			Transform(const Vector3f& pos, const Vector3f& scale);
			Matrix4x4f GetTransformMatrix()const;
		private:
			void UpdateTransformMatrix();
			//TODO : add quaternion to represent rotation
			Vector3f m_pos;
			Vector3f m_scale;
			Matrix4x4f m_transform;
		};
	}
}
