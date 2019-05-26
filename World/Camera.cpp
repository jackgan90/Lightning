#include "Camera.h"
#include "IRenderer.h"
#include "IRenderPlugin.h"
#define EPSILON 0.0001

namespace Lightning
{
	namespace World
	{
		using Foundation::Math::DegreesToRadians;
		using Foundation::Math::Vector4f;
		using Foundation::Math::Quaternionf;
		extern Plugins::IRenderPlugin* gRenderPlugin;
		Camera::Camera():mType(CameraType::Perspective), mNearPlane(0.1f), mFarPlane(1000.0f), 
			mFov(DegreesToRadians(60.0f)), mAspectRatio(1.0f)
		{
			UpdateProjectionMatrix();
		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp):
			mType(CameraType::Perspective), mNearPlane(0.1f), mFarPlane(1000.0f),
			mFov(DegreesToRadians(60.0f)), mAspectRatio(1.0f)
		{
			mTransform.SetPosition(worldPosition);
			mTransform.OrientTo(lookDir, worldUp);
			UpdateProjectionMatrix();
		}

		Camera::Camera( CameraType type, const float near, const float far,const float fov,const float aspectRatio,
			const Vector3f& worldPosition, 
			const Vector3f& lookDir,
			const Vector3f& worldUp):
			mType(CameraType::Perspective), mNearPlane(0.01f), mFarPlane(1000.0f), 
			mFov(DegreesToRadians(fov)),mAspectRatio(aspectRatio)
		{
			mTransform.SetPosition(worldPosition);
			mTransform.OrientTo(lookDir, worldUp);
			UpdateProjectionMatrix();
		}

		Camera::~Camera()
		{


		}

		void Camera::SetCameraType(CameraType type)
		{
			if (type == mType)
				return;
			mType = type;
			UpdateProjectionMatrix();
		}

		void Camera::SetNear(const float nearPlane)
		{
			mNearPlane = nearPlane;
			UpdateProjectionMatrix();
		}

		void Camera::SetFar(const float farPlane)
		{
			mFarPlane = farPlane;
			UpdateProjectionMatrix();
		}

		void Camera::SetFOV(const float fov)
		{
			if (std::abs(fov - mFov) < EPSILON)
				return;
			mFov = DegreesToRadians(fov);
			UpdateProjectionMatrix();
		}

		void Camera::SetAspectRatio(const float aspectRatio)
		{
			if (std::abs(aspectRatio - mAspectRatio) < EPSILON)
				return;
			mAspectRatio = aspectRatio;
			UpdateProjectionMatrix();
		}

		void Camera::UpdateProjectionMatrix()
		{
			mProjectionMatrix.SetIdentity();
			//auto ndcNear = gRenderPlugin->GetRenderer()->GetNDCNearPlane();
			//TODO £ºIf OpenGL is implemented,should adjust according to it
			switch (mType)
			{
			case CameraType::Perspective:
			{
				auto f = 1.0f / tan(mFov * 0.5f);
				auto d = mFarPlane - mNearPlane;
				mProjectionMatrix.SetCell(0, 0, f / mAspectRatio);
				mProjectionMatrix.SetCell(1, 1, f);
				mProjectionMatrix.SetCell(2, 2, mFarPlane / d);
				mProjectionMatrix.SetCell(3, 3, 0.0f);
				mProjectionMatrix.SetCell(2, 3, 1.0f);
				mProjectionMatrix.SetCell(3, 2, - mNearPlane * mFarPlane / d);
				break;
			}
			case CameraType::Orthographic:
			{
				auto h = 2 * tan(mFov / 2.0f) * mNearPlane;
				auto w = mAspectRatio * h;
				auto d = mFarPlane - mNearPlane;
				mProjectionMatrix.SetCell(0, 0, 2.0f / w);
				mProjectionMatrix.SetCell(1, 1, 2.0f / h);
				mProjectionMatrix.SetCell(2, 2, 1.0f / d);
				mProjectionMatrix.SetCell(3, 2, - mNearPlane / d);
				break;
			}
			default:
				break;
			}
		}
	}
}