#pragma once
#include <cmath>
#include "ICamera.h"
#include "Transform.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Transformer;
		//Use right-handed coordinate system
		class Camera : public ICamera
		{
		public:
			Camera();
			Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			Camera(CameraType type, const float near, const float far, const float fov, const float aspectRatio,
				const Vector3f& worldPosition = Vector3f::Zero(), 
				const Vector3f& lookDir = Vector3f{0.0f, 0.0f, -1.0f},
				const Vector3f& worldUp = Vector3f{0.0f, 1.0f, 0.0f});
			~Camera()override;
			Matrix4f INTERFACECALL GetViewMatrix()const override{ return mViewMatrix; }
			Matrix4f INTERFACECALL GetProjectionMatrix()const override{ return mProjectionMatrix; }
			Matrix4f INTERFACECALL GetInvViewMatrix()const override{ return mInvViewMatrix; }
			void INTERFACECALL MoveTo(const Vector3f& worldPosition)override;
			void INTERFACECALL LookAt(const Vector3f& lookPosition)override;
			void INTERFACECALL RotateTowards(const Vector3f& direction)override;
			void INTERFACECALL SetNear(const float nearPlane)override;
			void INTERFACECALL SetFar(const float farPlane)override;
			float INTERFACECALL GetNear()const override{ return mNearPlane; }
			float INTERFACECALL GetFar()const override{ return mFarPlane; }
			void INTERFACECALL SetCameraType(CameraType type)override;
			CameraType INTERFACECALL GetCameraType()const override{ return mType; }
			//Set vertical field of view in degrees
			void INTERFACECALL SetFOV(const float fov)override;
			float INTERFACECALL GetFOV()const override{ return Foundation::Math::RadiansToDegrees(mFov); }
			void INTERFACECALL SetAspectRatio(const float aspectRatio)override;
			float INTERFACECALL GetAspectRatio()const override{ return mAspectRatio; }
			Vector3f INTERFACECALL GetWorldPosition()const override{ return mTransformer.GetPosition(); }
			Vector3f INTERFACECALL CameraPointToWorld(const Vector3f& point)const override;
			Vector3f INTERFACECALL WorldPointToCamera(const Vector3f& point)const override;
			Vector3f INTERFACECALL CameraDirectionToWorld(const Vector3f& direction)const override;
			Vector3f INTERFACECALL WorldDirectionToCamera(const Vector3f& direction)const;
			Vector3f INTERFACECALL GetForward()const override{ return mTransformer.Forward(); }
			void INTERFACECALL SetRotation(const Quaternionf& rotation) override;
			Quaternionf INTERFACECALL GetRotation()const override{ return mTransformer.GetRotation(); }
			void INTERFACECALL Update(IRenderer*)override;
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
			Transformer mTransformer;
			Matrix4f mViewMatrix;
			Matrix4f mInvViewMatrix;
			Matrix4f mProjectionMatrix;
		};
	}
}
