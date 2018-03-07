#pragma once
#include <cmath>
#include "sceneexportdef.h"
#include "vector.h"

#define LIGHTNINGGE_PI 3.14159265358979

namespace LightningGE
{
	namespace Scene
	{
		enum class CameraType
		{
			Perspective,
			Orthographic,
		};
		inline float DegreesToRadians(const float degree)
		{
			return static_cast<float>(degree * LIGHTNINGGE_PI / 180.0);
		}
		inline float RadiansToDegrees(const float radians)
		{
			return static_cast<float>(radians * 180.0 / LIGHTNINGGE_PI);
		}
		using Render::Vector3f;
		using Render::Matrix4x4f;
		//Use right-handed coordinate system
		class LIGHTNINGGE_SCENE_API Camera
		{
		public:
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			Camera(CameraType type, const float near, const float far, const float fov, const float aspectRatio,
				const Vector3f& worldPosition = Vector3f::Zero(), 
				const Vector3f& lookDir = Vector3f{0.0f, 0.0f, -1.0f},
				const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			virtual ~Camera();
			Matrix4x4f GetViewMatrix()const { return m_viewMatrix; }
			Matrix4x4f GetProjectionMatrix()const { return m_projectionMatrix; }
			void MoveTo(const Vector3f& worldPosition);
			void LookAt(const Vector3f& worldPosition, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			void SetNear(const float nearPlane);
			void SetFar(const float farPlane);
			float GetNear()const { return m_nearPlane; }
			float GetFar()const { return m_farPlane; }
			void SetCameraType(CameraType type);
			CameraType GetCameraType()const { return m_type; }
			//Set vertical field of view in degrees
			void SetFOV(const float fov);
			float GetFOV()const { return RadiansToDegrees(m_fov); }
			void SetAspectRatio(const float aspectRatio);
			float GetAspectRatio()const { return m_aspectRatio; }
		protected:
			void UpdateViewMatrix();
			void UpdateProjectionMatrix();
			CameraType m_type;
			float m_nearPlane;
			float m_farPlane;
			//vertical fov
			float m_fov;
			//ratio of near plane width / near plane height
			float m_aspectRatio;
			Vector3f m_worldPosition;
			Vector3f m_xAxis;
			Vector3f m_yAxis;
			Vector3f m_zAxis;
			Matrix4x4f m_viewMatrix;
			Matrix4x4f m_projectionMatrix;
		};
	}
}
