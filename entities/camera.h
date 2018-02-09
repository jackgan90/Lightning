#include "entitiesexportdef.h"
#include "vector.h"

namespace LightningGE
{
	namespace Entities
	{
		using Render::Vector3f;
		using Render::Matrix4x4f;
		class LIGHTNINGGE_ENTITIES_API Camera
		{
		public:
			LIGHTNINGGE_ALIGNED_OPERATOR_NEW
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& worldForward, const Vector3f& worldUp);
			virtual ~Camera();
			Matrix4x4f GetViewMatrix()const;
		protected:
			Vector3f m_worldPosition;
			Vector3f m_worldForward;
			Vector3f m_worldUp;
		};
	}
}
