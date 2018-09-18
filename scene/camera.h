#pragma once
#include <cmath>
#include "sceneexportdef.h"
#include "matrix.h"
#include "vector.h"

#define LIGHTNING_PI 3.14159265358979

namespace Lightning
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
			return static_cast<float>(degree * LIGHTNING_PI / 180.0);
		}
		inline float RadiansToDegrees(const float radians)
		{
			return static_cast<float>(radians * 180.0 / LIGHTNING_PI);
		}
		using Render::Vector3f;
		using Render::Matrix4f;
		//Use right-handed coordinate system
		class LIGHTNING_SCENE_API Camera
		{
		public:
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			Camera(CameraType type, const float near, const float far, const float fov, const float aspectRatio,
				const Vector3f& worldPosition = Vector3f::Zero(), 
				const Vector3f& lookDir = Vector3f{0.0f, 0.0f, -1.0f},
				const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			virtual ~Camera();
			Matrix4f GetViewMatrix()const { return mViewMatrix; }
			Matrix4f GetProjectionMatrix()const { return mProjectionMatrix; }
			void MoveTo(const Vector3f& worldPosition);
			void LookAt(const Vector3f& worldPosition, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			void SetNear(const float nearPlane);
			void SetFar(const float farPlane);
			float GetNear()const { return mNearPlane; }
			float GetFar()const { return mFarPlane; }
			void SetCameraType(CameraType type);
			CameraType GetCameraType()const { return mType; }
			//Set vertical field of view in degrees
			void SetFOV(const float fov);
			float GetFOV()const { return RadiansToDegrees(mFov); }
			void SetAspectRatio(const float aspectRatio);
			float GetAspectRatio()const { return mAspectRatio; }
		protected:
			void UpdateViewMatrix();
			void UpdateProjectionMatrix();
			CameraType mType;
			float mNearPlane;
			float mFarPlane;
			//vertical fov
			float mFov;
			//ratio of near plane width / near plane height
			float mAspectRatio;
			Vector3f mWorldPosition;
			Vector3f mXAxis;
			Vector3f mYAxis;
			Vector3f mZAxis;
			Matrix4f mViewMatrix;
			Matrix4f mProjectionMatrix;
		};
	}
}
