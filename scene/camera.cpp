#include "camera.h"
#include "renderer.h"
#define EPSILON 0.0001

namespace LightningGE
{
	namespace Scene
	{
		using Render::Matrix3x3f;
		using Render::Vector4f;
		Camera::Camera():m_type(CameraType::Perspective), m_nearPlane(1.0f), m_farPlane(1000.0f), 
			m_fov(DegreesToRadians(60.0f)), m_aspectRatio(1.0f),
			m_worldPosition{0.0f, 0.0f, 0.0f}, 
			m_xAxis{1.0f, 0.0f, 0.0f}, m_yAxis{0.0f, 1.0f, 0.0f}, m_zAxis{0.0f, 0.0f, 1.0f}
		{
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp):
			m_type(CameraType::Perspective), m_nearPlane(1.0f), m_farPlane(1000.0f),
			m_fov(DegreesToRadians(60.0f)), m_aspectRatio(1.0f),
			m_worldPosition(worldPosition), 
			m_zAxis(-lookDir), m_xAxis(worldUp.Cross(m_zAxis)), m_yAxis(m_zAxis.Cross(m_xAxis))
		{
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::Camera( CameraType type, const float near, const float far,const float fov,const float aspectRatio,
			const Vector3f& worldPosition, 
			const Vector3f& lookDir,
			const Vector3f& worldUp):
			m_type(CameraType::Perspective), m_nearPlane(1.0f), m_farPlane(1000.0f), 
			m_fov(DegreesToRadians(fov)),m_aspectRatio(aspectRatio),
			m_worldPosition(worldPosition), 
			m_zAxis(-lookDir), m_xAxis(worldUp.Cross(m_zAxis)), m_yAxis(m_zAxis.Cross(m_xAxis))
		{
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::~Camera()
		{


		}

		void Camera::SetCameraType(CameraType type)
		{
			if (type == m_type)
				return;
			m_type = type;
			UpdateProjectionMatrix();
		}

		void Camera::MoveTo(const Vector3f& worldPosition)
		{
			m_worldPosition = worldPosition;
			UpdateViewMatrix();
		}

		void Camera::LookAt(const Vector3f& worldPosition, const Vector3f& worldUp)
		{
			auto normalizedUp = worldUp.Normalized();
			m_zAxis = m_worldPosition - worldPosition;
			m_zAxis.Normalize();
			m_xAxis = normalizedUp.Cross(m_zAxis);
			m_xAxis.Normalize();
			m_yAxis = m_zAxis.Cross(m_xAxis);
			UpdateViewMatrix();
		}

		void Camera::SetNear(const float nearPlane)
		{
			m_nearPlane = nearPlane;
			UpdateProjectionMatrix();
		}

		void Camera::SetFar(const float farPlane)
		{
			m_farPlane = farPlane;
			UpdateProjectionMatrix();
		}

		void Camera::SetFOV(const float fov)
		{
			if (std::abs(fov - m_fov) < EPSILON)
				return;
			m_fov = DegreesToRadians(fov);
			UpdateProjectionMatrix();
		}

		void Camera::SetAspectRatio(const float aspectRatio)
		{
			if (std::abs(aspectRatio - m_aspectRatio) < EPSILON)
				return;
			m_aspectRatio = DegreesToRadians(aspectRatio);
			UpdateProjectionMatrix();
		}

		void Camera::UpdateProjectionMatrix()
		{
			m_projectionMatrix.SetZero();
			auto ndcNear = Render::Renderer::Instance()->GetNDCNearPlane();
			switch (m_type)
			{
			case CameraType::Perspective:
			{
				m_projectionMatrix(0, 0) = static_cast<float>(1.0 / (tan(m_fov / 2.0) * m_aspectRatio));
				m_projectionMatrix(1, 1) = static_cast<float>(1.0 / tan(m_fov / 2.0));
				m_projectionMatrix(2, 2) = (m_nearPlane * ndcNear - m_farPlane) / (m_farPlane - m_nearPlane);
				m_projectionMatrix(2, 3) = (ndcNear - 1.0f) * m_nearPlane * m_farPlane / (m_farPlane - m_nearPlane);
				m_projectionMatrix(3, 2) = -1;
				break;
			}
			case CameraType::Orthographic:
			{
				m_projectionMatrix(0, 0) = static_cast<float>(1.0 / (tan(m_fov / 2.0) * m_aspectRatio * m_nearPlane));
				m_projectionMatrix(1, 1) = static_cast<float>(1.0 / (tan(m_fov / 2.0) * m_nearPlane));
				m_projectionMatrix(2, 2) = static_cast<float>(-2.0 / (m_farPlane - m_nearPlane));
				m_projectionMatrix(2, 3) = -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane);
				m_projectionMatrix(3, 2) = 0;
				break;
			}
			default:
				break;
			}
		}

		void Camera::UpdateViewMatrix()
		{
			/*
				V =	| R 0 |
					| 0 1 |
				T =	| I A |
					| 0 1 |
			view matrix is the multiplication of VT
					| R RA|
					| 0 1 |
			*/	
			Vector3f translation{ -m_worldPosition[0], -m_worldPosition[1], -m_worldPosition[2] };
			Vector4f row0(m_xAxis);
			row0[3] = m_xAxis.Dot(translation);
			Vector4f row1(m_yAxis);
			row1[3] = m_yAxis.Dot(translation);
			Vector4f row2(m_zAxis);
			row2[3] = m_zAxis.Dot(translation);
			m_viewMatrix.SetRows(0, row0, row1, row2, Vector4f({0.0f, 0.0f, 0.0f, 1.0f}));

		}
	}
}