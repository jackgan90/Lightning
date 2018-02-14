#include "camera.h"

namespace LightningGE
{
	namespace Entities
	{
		Camera::Camera():m_worldPosition{0.0f, 0.0f, 0.0f}, 
			m_xAxis{1.0f, 0.0f, 0.0f}, m_yAxis{0.0f, 0.1f, 0.0f}, m_zAxis{0.0f, 0.0f, -1.0f}
		{
			UpdateViewMatrix();
		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp):
			m_worldPosition(worldPosition), m_zAxis(-lookDir), m_xAxis(worldUp.Cross(m_zAxis)), m_yAxis(m_zAxis.Cross(m_xAxis))
		{
			UpdateViewMatrix();
		}

		Camera::~Camera()
		{


		}

		void Camera::MoveTo(const Vector3f& worldPosition)
		{
			m_worldPosition = worldPosition;
			UpdateViewMatrix();
		}

		void Camera::LookAt(const Vector3f& worldPosition, const Vector3f& worldUp)
		{
			//m_zAxis = m_worldPosition - worldPosition;
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