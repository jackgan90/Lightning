#include "camera.h"

namespace LightningGE
{
	namespace Entities
	{
		Camera::Camera():m_worldPosition{0.0f, 0.0f, 0.0f}, m_worldForward{0.0f, 0.0f, 1.0f}, m_worldUp{0.0f, 1.0f, 0.0f}
		{

		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& worldForward, const Vector3f& worldUp):
			m_worldPosition(worldPosition), m_worldForward(worldForward), m_worldUp(worldUp)
		{

		}

		Camera::~Camera()
		{


		}

		Matrix4x4f Camera::GetViewMatrix()const
		{
		}
	}
}