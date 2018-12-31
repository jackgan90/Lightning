#include "Camera.h"
#include "IRenderer.h"
#include "IRenderPlugin.h"
#define EPSILON 0.0001

namespace Lightning
{
	namespace Scene
	{
		using Foundation::Math::DegreesToRadians;
		using Foundation::Math::Vector4f;
		using Foundation::Math::Quaternionf;
		extern Plugins::IRenderPlugin* gRenderPlugin;
		Camera::Camera():mType(CameraType::Perspective), mNearPlane(0.01f), mFarPlane(1000.0f), 
			mFov(DegreesToRadians(60.0f)), mAspectRatio(1.0f),
			mTransformer(Vector3f::Zero(), Vector3f{1, 1, 1}, Quaternionf::Identity())
		{
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::Camera(const Vector3f& worldPosition, const Vector3f& lookDir, const Vector3f& worldUp):
			mType(CameraType::Perspective), mNearPlane(0.01f), mFarPlane(1000.0f),
			mFov(DegreesToRadians(60.0f)), mAspectRatio(1.0f)
		{
			mTransformer.SetPosition(worldPosition);
			mTransformer.OrientTo(lookDir, worldUp);
			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}

		Camera::Camera( CameraType type, const float near, const float far,const float fov,const float aspectRatio,
			const Vector3f& worldPosition, 
			const Vector3f& lookDir,
			const Vector3f& worldUp):
			mType(CameraType::Perspective), mNearPlane(0.01f), mFarPlane(1000.0f), 
			mFov(DegreesToRadians(fov)),mAspectRatio(aspectRatio)
		{
			mTransformer.SetPosition(worldPosition);
			mTransformer.OrientTo(lookDir, worldUp);
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
			mTransformer.SetPosition(worldPosition);
			UpdateViewMatrix();
		}

		void Camera::RotateTowards(const Vector3f& direction)
		{
			mTransformer.OrientTo(direction, Vector3f::up());
			UpdateViewMatrix();
		}

		void Camera::LookAt(const Vector3f& lookPosition)
		{
			mTransformer.LookAt(lookPosition, Vector3f::up());
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
			mAspectRatio = aspectRatio;
			UpdateProjectionMatrix();
		}

		void Camera::UpdateProjectionMatrix()
		{
			mProjectionMatrix.SetZero();
			auto ndcNear = gRenderPlugin->GetRenderer()->GetNDCNearPlane();
			switch (mType)
			{
			case CameraType::Perspective:
			{
				mProjectionMatrix.SetCell(0, 0, static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio)));
				mProjectionMatrix.SetCell(1, 1, static_cast<float>(1.0 / tan(mFov / 2.0)));
				mProjectionMatrix.SetCell(2, 2, (mNearPlane * ndcNear - mFarPlane) / (mFarPlane - mNearPlane));
				mProjectionMatrix.SetCell(3, 2, -1);
				mProjectionMatrix.SetCell(2, 3, (ndcNear - 1.0f) * mNearPlane * mFarPlane / (mFarPlane - mNearPlane));
				break;
			}
			case CameraType::Orthographic:
			{
				mProjectionMatrix.SetCell(0, 0, static_cast<float>(1.0 / (tan(mFov / 2.0) * mAspectRatio * mNearPlane)));
				mProjectionMatrix.SetCell(1, 1, static_cast<float>(1.0 / (tan(mFov / 2.0) * mNearPlane)));
				mProjectionMatrix.SetCell(2, 2, static_cast<float>(-2.0 / (mFarPlane - mNearPlane)));
				mProjectionMatrix.SetCell(2, 3, -(mFarPlane + mNearPlane) / (mFarPlane - mNearPlane));
				mProjectionMatrix.SetCell(3, 3, 1);
				break;
			}
			default:
				break;
			}
		}

		void Camera::SetRotation(const Quaternionf& rotation)
		{
			mTransformer.SetRotation(rotation);
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
			//static int cnt = 0;
			//if (cnt >= 3)
			//	return;
			//cnt++;
			mViewMatrix = mTransformer.GlobalToLocalMatrix4();
			mInvViewMatrix = mTransformer.LocalToGlobalMatrix4();

			//auto res = mViewMatrix * mInvViewMatrix;
		}

		Vector3f Camera::CameraPointToWorld(const Vector3f& point)const
		{
			auto res = mInvViewMatrix * Vector4f{point.x, point.y, point.z, 1.0f};
			return Vector3f{ res.x, res.y, res.z };
		}

		Vector3f Camera::WorldPointToCamera(const Vector3f& point)const
		{
			auto res = mViewMatrix * Vector4f{point.x, point.y, point.z, 1.0f};
			return Vector3f{ res.x, res.y, res.z };
		}

		Vector3f Camera::CameraDirectionToWorld(const Vector3f& direction)const
		{
			Vector4f dir{direction.x, direction.y, direction.z, 0.0f};
			auto res = mInvViewMatrix * dir;
			return Vector3f{ res.x, res.y, res.z };
		}

		Vector3f Camera::WorldDirectionToCamera(const Vector3f& direction)const
		{
			Vector4f dir{direction.x, direction.y, direction.z, 0.0f};
			auto res = mViewMatrix * dir;
			return Vector3f{ res.x, res.y, res.z };
		}

		void Camera::Update(IRenderer* renderer)
		{
			auto window = renderer->GetOutputWindow();
			if (window)
			{
				auto width = window->GetWidth();
				auto height = window->GetHeight();
				auto aspectRatio = float(width) / height;
				SetAspectRatio(aspectRatio);
			}
		}
	}
}