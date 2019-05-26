#pragma once
#include "Math/Quaternion.h"

namespace Lightning
{
	namespace Render
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
			virtual ~ICamera() = default;
			virtual Matrix4f GetViewMatrix()const = 0;
			virtual Matrix4f GetProjectionMatrix()const = 0;
			virtual Matrix4f GetInvViewMatrix()const = 0;
			virtual void SetNear(const float nearPlane) = 0;
			virtual void SetFar(const float farPlane) = 0;
			virtual float GetNear()const = 0;
			virtual float GetFar()const = 0;
			virtual void SetCameraType(CameraType type) = 0;
			virtual CameraType GetCameraType()const = 0;
			//Set vertical field of view in degrees
			virtual void SetFOV(const float fov) = 0;
			virtual float GetFOV()const = 0;
			virtual void SetAspectRatio(const float aspectRatio) = 0;
			virtual float GetAspectRatio()const = 0;
		};
	}
}