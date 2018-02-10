#include "camera.h"

namespace LightningGE
{
	namespace Entities
	{
		Camera::Camera():m_worldPosition{0.0f, 0.0f, 0.0f}, m_worldForward{0.0f, 0.0f, 1.0f}, m_worldUp{0.0f, 1.0f, 0.0f}
		{
			UpdateViewMatrix();
		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& worldForward, const Vector3f& worldUp):
			m_worldPosition(worldPosition), m_worldForward(worldForward), m_worldUp(worldUp)
		{
			UpdateViewMatrix();
		}

		Camera::~Camera()
		{


		}

		void Camera::MoveTo(const Vector3f& worldPosition)
		{

		}

		void Camera::LookAt(const Vector3f& worldPosition, const Vector3f& worldUp)
		{

		}

		Matrix4x4f Camera::GetViewMatrix()const
		{
			return m_viewMatrix;
		}

		void Camera::UpdateViewMatrix()
		{

		}
	}
}