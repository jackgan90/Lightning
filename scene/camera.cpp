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

		void Camera::RotateTowards(const Vector3f& direction, const Vector3f& worldUp)
		{
			if (direction.IsZero())
				return;
			if (worldUp.IsZero())
				return;
			mZAxis = -direction;
			mZAxis.Normalize();
			UpdateXZAxis(worldUp.Normalized());
		}

		void Camera::LookAt(const Vector3f& lookPosition, const Vector3f& worldUp)
		{
			if (worldUp.IsZero())
				return;
			auto direction = mWorldPosition - lookPosition;
			if (direction.IsZero())
				return;
			mZAxis = direction;
			mZAxis.Normalize();
			UpdateXZAxis(worldUp.Normalized());
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
				mProjectionMatrix.SetCell(0, 0, static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio)));
				mProjectionMatrix.SetCell(1, 1, static_cast<float>(1.0 / tan(mFov / 2.0)));
				mProjectionMatrix.SetCell(2, 2, (mNearPlane * ndcNear - mFarPlane) / (mFarPlane - mNearPlane));
				mProjectionMatrix.SetCell(3, 2, -1);
				mProjectionMatrix.SetCell(2, 3, (ndcNear - 1.0f) * mNearPlane * mFarPlane / (mFarPlane - mNearPlane));
				/*
				mProjectionMatrix.m[0] = static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio));
				mProjectionMatrix.m[5] = static_cast<float>(1.0 / tan(mFov / 2.0));
				mProjectionMatrix.m[10] = (mNearPlane * ndcNear - mFarPlane) / (mFarPlane - mNearPlane);
				mProjectionMatrix.m[11] = -1;
				mProjectionMatrix.m[14] = (ndcNear - 1.0f) * mNearPlane * mFarPlane / (mFarPlane - mNearPlane);
				*/
				break;
			}
			case CameraType::Orthographic:
			{
				mProjectionMatrix.SetCell(0, 0, static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio * mNearPlane)));
				mProjectionMatrix.SetCell(1, 1, static_cast<float>(1.0 / (tan(mFov / 2.0) * mNearPlane)));
				mProjectionMatrix.SetCell(2, 2, static_cast<float>(-2.0 / (mFarPlane - mNearPlane)));
				mProjectionMatrix.SetCell(2, 3, -(mFarPlane + mNearPlane) / (mFarPlane - mNearPlane));
				mProjectionMatrix.SetCell(3, 3, 1);
				/*
				mProjectionMatrix.m[0] = static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio * mNearPlane));
				mProjectionMatrix.m[5] = static_cast<float>(1.0 / (tan(mFov / 2.0) * mNearPlane));
				mProjectionMatrix.m[10] = static_cast<float>(-2.0 / (mFarPlane - mNearPlane));
				mProjectionMatrix.m[14] =-(mFarPlane + mNearPlane) / (mFarPlane - mNearPlane);
				mProjectionMatrix.m[15] = 1;
				*/
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
			mViewMatrix.SetRow(0, row0);
			mViewMatrix.SetRow(1, row1);
			mViewMatrix.SetRow(2, row2);
			mViewMatrix.SetRow(3, Vector4f({0.0f, 0.0f, 0.0f, 1.0f}));

			Vector4f col0(mXAxis), col1(mYAxis), col2(mZAxis), col3(mWorldPosition);
			col0[3] = col1[3] = col2[3] = 0;
			mInvViewMatrix.SetColumn(0, col0);
			mInvViewMatrix.SetColumn(1, col1);
			mInvViewMatrix.SetColumn(2, col2);
			mInvViewMatrix.SetColumn(3, col3);
		}

		void Camera::UpdateXZAxis(const Vector3f& up)
		{
			auto newXAxis = up.Cross(mZAxis);
			bool xAxisUpdated{ false };
			if (!newXAxis.IsZero())	//mXAxis is zero when up and z axis are aligned, in this case, just keep X Axis unchanged
			{
				mXAxis = newXAxis;
				mXAxis.Normalize();
				xAxisUpdated = true;
			}
			mYAxis = mZAxis.Cross(mXAxis);
			//If x-axis remains unchanged,new z axis and x axis may not be perpendicular.So we must
			//recalculate x axis based on new y and z axis
			if (!xAxisUpdated)	
			{
				mXAxis = mYAxis.Cross(mZAxis);
			}
			UpdateViewMatrix();
		}

		Vector3f Camera::CameraPointToWorld(const Vector3f& point)const
		{
			return mInvViewMatrix * Vector4f(point);
		}

		Vector3f Camera::WorldPointToCamera(const Vector3f& point)const
		{
			return mViewMatrix * Vector4f(point);
		}

		Vector3f Camera::CameraDirectionToWorld(const Vector3f& direction)const
		{
			Vector4f dir(direction);
			dir.w = 0;
			return mInvViewMatrix * dir;
		}

		Vector3f Camera::WorldDirectionToCamera(const Vector3f& direction)const
		{
			Vector4f dir(direction);
			dir.w = 0;
			return mViewMatrix * dir;
		}
	}
}