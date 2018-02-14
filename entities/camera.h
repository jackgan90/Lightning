#include "entitiesexportdef.h"
#include "vector.h"

namespace LightningGE
{
	namespace Entities
	{
		using Render::Vector3f;
		using Render::Matrix4x4f;
		//Use right-handed coordinate system
		class LIGHTNINGGE_ENTITIES_API Camera
		{
		public:
			LIGHTNINGGE_ALIGNED_OPERATOR_NEW
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			virtual ~Camera();
			Matrix4x4f GetViewMatrix()const;
			void MoveTo(const Vector3f& worldPosition);
			void LookAt(const Vector3f& worldPosition, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
		protected:
			void UpdateViewMatrix();
			Vector3f m_worldPosition;
			Vector3f m_xAxis;
			Vector3f m_yAxis;
			Vector3f m_zAxis;
			Matrix4x4f m_viewMatrix;
		};
	}
}
