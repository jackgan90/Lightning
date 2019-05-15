#pragma once
#include <cassert>
#include <cmath>
#include <limits>
#include "Common.h"
#include "Matrix.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			template<typename T>
			struct EulerAngle
			{
				static_assert(std::is_pod<T>::value, "T is not a POD for EulerAngle!");
				T yaw, roll, pitch;
			};
			using EulerAnglef = EulerAngle<float>;
			static_assert(std::is_pod<EulerAnglef>::value, "EulerAnglef is not a POD!");

			template<typename T>
			struct Quaternion
			{
				static_assert(std::is_pod<T>::value, "T is not a POD!");

				T Length()const
				{
					return std::sqrt(x * x + y * y + z * z + w * w);
				}

				void Normalize()
				{
					auto length = Length();
					if (length < std::numeric_limits<T>::epsilon())
						return;
					x /= length;
					y /= length;
					z /= length;
					w /= length;
				}

				Quaternion<T> Inversed()const
				{
					Quaternion<T> q{-x, -y, -z, w};
					auto sqrl = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
					assert(sqrl > 0);
					q.x /= sqrl;
					q.y /= sqrl;
					q.z /= sqrl;
					q.w /= sqrl;
					return q;
				}

				Quaternion<T> operator*(const Quaternion<T>& q)const
				{
					Quaternion<T> res;
					res.w = w * q.w - x * q.x - y * q.y - z * q.z;
					res.x = w * q.x + x * q.w + y * q.z - z * q.y;
					res.y = w * q.y - x * q.z + y * q.w + z * q.x;
					res.z = w * q.z + x * q.y - y * q.x + z * q.w;
					return res;
				}

				Quaternion<T>& operator*=(const Quaternion<T>& q)
				{
					*this = *this * q;
					return *this;
				}
				
				//ref : https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
				void FromEulerAngle(const EulerAngle<T>& angle)
				{
					auto cos_roll = std::cos(angle.roll * 0.5);
					auto sin_roll = std::sin(angle.roll * 0.5);
					auto cos_pitch = std::cos(angle.pitch * 0.5);
					auto sin_pitch = std::sin(angle.pitch * 0.5);
					auto cos_yaw = std::cos(angle.yaw * 0.5);
					auto sin_yaw = std::sin(angle.yaw * 0.5);

					w = T(cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw);
					x = T(cos_roll * sin_pitch * cos_yaw - sin_roll * cos_pitch * sin_yaw);
					y = T(cos_roll * cos_pitch * sin_yaw + sin_roll * sin_pitch * cos_yaw);
					z = T(sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw);
				}

				void FromAxisAndAngle(Vector3<T> direction, T theta)
				{
					direction.Normalize();
					auto sint = std::sin(theta / T(2.0));
					x = sint * direction.x;
					y = sint * direction.y;
					z = sint * direction.z;
					w = std::cos(theta / T(2.0));
				}

				//ref : https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
				void ToEulerAngle(EulerAngle<T>& angle)
				{
					// pitch (x-axis rotation)
					T sinr_cosp = +2.0 * (w * x + y * z);
					T cosr_cosp = +1.0 - 2.0 * (x * x + y * y);
					angle.pitch = std::atan2(sinr_cosp, cosr_cosp);

					// yaw (y-axis rotation)
					T sinp = +2.0 * (w * y - z * x);
					if (fabs(sinp) >= 1)
						angle.yaw = std::copysign(Math::PI / 2, sinp); // use 90 degrees if out of range
					else
						angle.yaw = std::asin(sinp);

					// roll (z-axis rotation)
					T siny_cosp = +2.0 * (w * z + x * y);
					T cosy_cosp = +1.0 - 2.0 * (y * y + z * z);  
					angle.roll = std::atan2(siny_cosp, cosy_cosp);
				}

				//ref : https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
				Vector3<T> RotateVector(const Vector3<T>& v)const
				{ 
					// Extract the vector part of the quaternion
					Vector3<T> u{x, y, z};

					// Do the math
					return T(2.0) * u.Dot(v) * u + (w*w - u.Dot(u)) * v + T(2.0) * w * u.Cross(v);
				}

				Vector3<T> operator*(const Vector3<T>& v)const
				{
					return RotateVector(v);
				}

				
				//construct a rotation based on source and dest directions.Both source and dest are unit vector.
				//ref : https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
				static Quaternion<T> MakeRotation(Vector3<T> source, Vector3<T> dest)
				{
					source.Normalize();
					dest.Normalize();
					auto dot = source.Dot(dest);

					Vector3<T> rotAxis;
					if (dot < -1 + T(0.0001))
					{
						// special case when vectors in opposite directions :
						// there is no "ideal" rotation axis
						// So guess one; any will do as long as it's perpendicular to start
						// This implementation favors a rotation around the Up axis,
						// since it's often what you want to do.
						rotAxis = Vector3<T>::forward().Cross(source);
						if (rotAxis.IsZero())// bad luck, they were parallel, try again!
						{
							rotAxis = Vector3<T>::right().Cross(source);
						}
						rotAxis.Normalize();
						Quaternion<T> res;
						res.FromAxisAndAngle(rotAxis, T(PI));
						return res;
					}
					else
					{
						rotAxis = source.Cross(dest);
						auto s = std::sqrt((1 + dot) * 2.0);
						auto invs = 1 / s;
						return Quaternion<T>{T(rotAxis.x * invs), T(rotAxis.y * invs), T(rotAxis.z * invs), T(s * 0.5)};
					}
				}

				//ref : Mathematics for 3D Game Programming And Computer Graphics
				void ToMatrix(Matrix4<T>& mat)const
				{
					mat.SetIdentity();
					
					T x2 = 2 * x * x;
					T y2 = 2 * y * y;
					T z2 = 2 * z * z;
					T xy = 2 * x * y;
					T xz = 2 * x * z;
					T wx = 2 * x * w;
					T yz = 2 * y * z;
					T wy = 2 * y * w;
					T wz = 2 * z * w;
					
					mat.SetCell(0, 0, 1 - y2 - z2);
					mat.SetCell(1, 0, xy - wz);
					mat.SetCell(2, 0, xz + wy);

					mat.SetCell(0, 1, xy + wz);
					mat.SetCell(1, 1, 1 - x2 - z2);
					mat.SetCell(2, 1, yz - wx);

					mat.SetCell(0, 2, xz - wy);
					mat.SetCell(1, 2, yz + wx);
					mat.SetCell(2, 2, 1 - x2 - y2);
				}

				static const Quaternion& Identity()
				{
					static Quaternion quat;
					quat.x = quat.y = quat.z = T(0);
					quat.w = T(1);
					return quat;
				}
				T x, y, z, w;
			};

			using Quaternionf = Quaternion<float>;
			static_assert(std::is_pod<Quaternionf>::value, "Quaternionf is not a POD!");
		}
	}
}
