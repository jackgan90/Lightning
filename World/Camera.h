#pragma once
#include <cmath>
#include "ISpaceCamera.h"
#include "SpaceObject.h"

namespace Lightning
{
	namespace World
	{
		using Foundation::Math::Transform;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Quaternionf;
		using Render::CameraType;
		//Use left-handed coordinate system
		class Camera : public SpaceObject<ISpaceCamera, Camera>
		{
		public:
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			Camera(CameraType type, const float near, const float far, const float fov, const float aspectRatio,
				const Vector3f& worldPosition = Vector3f::Zero(), 
				const Vector3f& lookDir = Vector3f{0.0f, 0.0f, 1.0f},
				const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			~Camera()override;
			Matrix4f GetViewMatrix()const override{ return mTransform.GlobalToLocalMatrix4(); }
			Matrix4f GetProjectionMatrix()const override{ return mProjectionMatrix; }
			Matrix4f GetInvViewMatrix()const override{ return mTransform.LocalToGlobalMatrix4(); }
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
		protected:
			void UpdateProjectionMatrix();
			CameraType mType;
			float mNearPlane;
			float mFarPlane;
			//vertical fov
			float mFov;
			//ratio of near plane width / near plane height
			float mAspectRatio;
			Matrix4f mProjectionMatrix;
		};
	}
}
