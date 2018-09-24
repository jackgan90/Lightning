#include "camera.h"
#include "renderer.h"
#define EPSILON 0.0001

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::DegreesToRadians;
		using Foundation::Math::Vector4f;
		Camera::Camera():mType(CameraType::Perspective), mNearPlane(1.0f), mFarPlane(1000.0f), 
			mFov(DegreesToRadians(60.0f)), mAspectRatio(1.0f),
			mWorldPosition{0.0f, 0.0f, 0.0f}, 
			mXAxis{1.0f, 0.0f, 0.0f}, mYAxis{0.0f, 1.0f, 0.0f}, mZAxis{0.0f, 0.0f, 1.0f}
		{
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp):
			mType(CameraType::Perspective), mNearPlane(1.0f), mFarPlane(1000.0f),
			mFov(DegreesToRadians(60.0f)), mAspectRatio(1.0f),
			mWorldPosition(worldPosition), 
			mZAxis(-lookDir), mXAxis(worldUp.Cross(mZAxis)), mYAxis(mZAxis.Cross(mXAxis))
		{
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::Camera( CameraType type, const float near, const float far,const float fov,const float aspectRatio,
			const Vector3f& worldPosition, 
			const Vector3f& lookDir,
			const Vector3f& worldUp):
			mType(CameraType::Perspective), mNearPlane(1.0f), mFarPlane(1000.0f), 
			mFov(DegreesToRadians(fov)),mAspectRatio(aspectRatio),
			mWorldPosition(worldPosition), 
			mZAxis(-lookDir), mXAxis(worldUp.Cross(mZAxis)), mYAxis(mZAxis.Cross(mXAxis))
		{
			UpdateViewMatrix();
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

		void Camera::MoveTo(const Vector3f& worldPosition)
		{
			mWorldPosition = worldPosition;
			UpdateViewMatrix();
		}

		void Camera::LookAt(const Vector3f& worldPosition, const Vector3f& worldUp)
		{
			auto normalizedUp = worldUp.Normalized();
			mZAxis = mWorldPosition - worldPosition;
			mZAxis.Normalize();
			mXAxis = normalizedUp.Cross(mZAxis);
			mXAxis.Normalize();
			mYAxis = mZAxis.Cross(mXAxis);
			UpdateViewMatrix();
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
			mAspectRatio = DegreesToRadians(aspectRatio);
			UpdateProjectionMatrix();
		}

		void Camera::UpdateProjectionMatrix()
		{
			mProjectionMatrix.SetZero();
			auto ndcNear = Render::Renderer::Instance()->GetNDCNearPlane();
			switch (mType)
			{
			case CameraType::Perspective:
			{
				mProjectionMatrix.m[0] = static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio));
				mProjectionMatrix.m[5] = static_cast<float>(1.0 / tan(mFov / 2.0));
				mProjectionMatrix.m[10] = (mNearPlane * ndcNear - mFarPlane) / (mFarPlane - mNearPlane);
				mProjectionMatrix.m[11] = (ndcNear - 1.0f) * mNearPlane * mFarPlane / (mFarPlane - mNearPlane);
				mProjectionMatrix.m[14] = -1;
				break;
			}
			case CameraType::Orthographic:
			{
				mProjectionMatrix.m[0] = static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio * mNearPlane));
				mProjectionMatrix.m[5] = static_cast<float>(1.0 / (tan(mFov / 2.0) * mNearPlane));
				mProjectionMatrix.m[10] = static_cast<float>(-2.0 / (mFarPlane - mNearPlane));
				mProjectionMatrix.m[11] = -(mFarPlane + mNearPlane) / (mFarPlane - mNearPlane);
				mProjectionMatrix.m[14] = 0;
				break;
			}
			default:
				break;
			}
		}

		void Camera::SetWorldPosition(const Vector3f& position)
		{
			mWorldPosition = position;
			UpdateViewMatrix();
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
			Vector3f translation{ -mWorldPosition[0], -mWorldPosition[1], -mWorldPosition[2] };
			Vector4f row0(mXAxis);
			row0[3] = mXAxis.Dot(translation);
			Vector4f row1(mYAxis);
			row1[3] = mYAxis.Dot(translation);
			Vector4f row2(mZAxis);
			row2[3] = mZAxis.Dot(translation);
			mViewMatrix.SetRow(row0, 0);
			mViewMatrix.SetRow(row1, 1);
			mViewMatrix.SetRow(row2, 2);
			mViewMatrix.SetRow(Vector4f({0.0f, 0.0f, 0.0f, 1.0f}), 3);

		}
	}
}