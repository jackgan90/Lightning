#pragma once
#include <cmath>
#include "ICamera.h"
#include "Transform.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Transform;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Quaternionf;
		using Render::CameraType;
		//Use right-handed coordinate system
		class Camera : public Render::ICamera
		{
		public:
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			Camera(CameraType type, const float near, const float far, const float fov, const float aspectRatio,
				const Vector3f& worldPosition = Vector3f::Zero(), 
				const Vector3f& lookDir = Vector3f{0.0f, 0.0f, -1.0f},
				const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			~Camera()override;
			Matrix4f GetViewMatrix()const override{ return mViewMatrix; }
			Matrix4f GetProjectionMatrix()const override{ return mProjectionMatrix; }
			Matrix4f GetInvViewMatrix()const override{ return mInvViewMatrix; }
			void MoveTo(const Vector3f& worldPosition)override;
			void LookAt(const Vector3f& lookPosition)override;
			void RotateTowards(const Vector3f& direction)override;
			void SetNear(const float nearPlane)override;
			void SetFar(const float farPlane)override;
			float GetNear()const override{ return mNearPlane; }
			float GetFar()const override{ return mFarPlane; }
			void SetCameraType(CameraType type)override;
			CameraType GetCameraType()const override{ return mType; }
			//Set vertical field of view in degrees
			void SetFOV(const float fov)override;
			float GetFOV()const override{ return Foundation::Math::RadiansToDegrees(mFov); }
			void SetAspectRatio(const float aspectRatio)override;
			float GetAspectRatio()const override{ return mAspectRatio; }
			Vector3f GetWorldPosition()const override{ return mTransform.GetPosition(); }
			Vector3f CameraPointToWorld(const Vector3f& point)const override;
			Vector3f WorldPointToCamera(const Vector3f& point)const override;
			Vector3f CameraDirectionToWorld(const Vector3f& direction)const override;
			Vector3f WorldDirectionToCamera(const Vector3f& direction)const;
			Vector3f GetForward()const override{ return mTransform.Forward(); }
			void SetRotation(const Quaternionf& rotation) override;
			Quaternionf GetRotation()const override{ return mTransform.GetRotation(); }
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
