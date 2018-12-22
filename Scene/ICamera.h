#pragma once
#include "Math/Quaternion.h"
#include "Portable.h"

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::Matrix4f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Quaternionf;

		enum class CameraType
		{
			Perspective,
			Orthographic,
		};

		struct ICamera
		{
			virtual INTERFACECALL ~ICamera() = default;
			virtual Matrix4f INTERFACECALL GetViewMatrix()const = 0;
			virtual Matrix4f INTERFACECALL GetProjectionMatrix()const = 0;
			virtual Matrix4f INTERFACECALL GetInvViewMatrix()const = 0;
			virtual void INTERFACECALL MoveTo(const Vector3f& worldPosition) = 0;
			virtual void INTERFACECALL LookAt(const Vector3f& lookPosition) = 0;
			virtual void INTERFACECALL RotateTowards(const Vector3f& direction) = 0;
			virtual void INTERFACECALL SetNear(const float nearPlane) = 0;
			virtual void INTERFACECALL SetFar(const float farPlane) = 0;
			virtual float INTERFACECALL GetNear()const = 0;
			virtual float INTERFACECALL GetFar()const = 0;
			virtual void INTERFACECALL SetCameraType(CameraType type) = 0;
			virtual CameraType INTERFACECALL GetCameraType()const = 0;
			//Set vertical field of view in degrees
			virtual void INTERFACECALL SetFOV(const float fov) = 0;
			virtual float INTERFACECALL GetFOV()const = 0;
			virtual void INTERFACECALL SetAspectRatio(const float aspectRatio) = 0;
			virtual float INTERFACECALL GetAspectRatio()const = 0;
			virtual Vector3f INTERFACECALL GetWorldPosition()const = 0;
			virtual Vector3f INTERFACECALL CameraPointToWorld(const Vector3f& point)const = 0;
			virtual Vector3f INTERFACECALL WorldPointToCamera(const Vector3f& point)const = 0;
			virtual Vector3f INTERFACECALL CameraDirectionToWorld(const Vector3f& direction)const = 0;
			virtual Vector3f INTERFACECALL WorldDirectionToCamera(const Vector3f& direction)const = 0;
			virtual Vector3f INTERFACECALL GetForward()const = 0;
			virtual void INTERFACECALL SetRotation(const Quaternionf& rotation) = 0;
			virtual Quaternionf INTERFACECALL GetRotation()const = 0;
		};
	}
}