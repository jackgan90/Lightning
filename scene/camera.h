#pragma once
#include <cmath>
#include "sceneexportdef.h"
#include "transform.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Vector3f;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Quaternionf;
		using Render::Transform;
		enum class CameraType
		{
			Perspective,
			Orthographic,
		};
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
			Matrix4f GetInvViewMatrix()const { return mInvViewMatrix; }
			void MoveTo(const Vector3f& worldPosition);
			void LookAt(const Vector3f& lookPosition);
			void RotateTowards(const Vector3f& direction);
			void SetNear(const float nearPlane);
			void SetFar(const float farPlane);
			float GetNear()const { return mNearPlane; }
			float GetFar()const { return mFarPlane; }
			void SetCameraType(CameraType type);
			CameraType GetCameraType()const { return mType; }
			//Set vertical field of view in degrees
			void SetFOV(const float fov);
			float GetFOV()const { return Foundation::Math::RadiansToDegrees(mFov); }
			void SetAspectRatio(const float aspectRatio);
			float GetAspectRatio()const { return mAspectRatio; }
			Vector3f GetWorldPosition()const { return mTransform.GetPosition(); }
			Vector3f CameraPointToWorld(const Vector3f& point)const;
			Vector3f WorldPointToCamera(const Vector3f& point)const;
			Vector3f CameraDirectionToWorld(const Vector3f& direction)const;
			Vector3f WorldDirectionToCamera(const Vector3f& direction)const;
			Vector3f GetForward()const { return mTransform.Yaw(); }
			void SetRotation(const Quaternionf& rotation); 
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
			Transform mTransform;
			Matrix4f mViewMatrix;
			Matrix4f mInvViewMatrix;
			Matrix4f mProjectionMatrix;
		};
	}
}
