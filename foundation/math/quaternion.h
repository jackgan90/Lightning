#pragma once
#include <cassert>
#include <cmath>
#include <limits>
#include "common.h"
#include "matrix.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			template<typename T>
			struct EulerAngle
			{
				EulerAngle() : yaw(0), roll(0), pitch(0){}
				EulerAngle(T _yaw, T _roll, T _pitch) : yaw(_yaw), roll(_roll), pitch(_pitch){}
				T yaw, roll, pitch;
			};

			template<typename T>
			struct Quaternion : PlainObject<Quaternion<T>>
			{
				Quaternion() : x(0), y(0), z(0), w(1){}
				Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w){}
				//direction is a normalized vector3
				Quaternion(const Vector3<T>& direction, T theta)
				{
					FromAxisAndAngle(direction, theta);
				}

				Quaternion(const EulerAngle<T>& angle)
				{
					FromEulerAngle(angle);
				}

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

				void FromAxisAndAngle(const Vector3<T>& direction, T theta)
				{
					assert(direction.Length() >= 0.999 && direction.Length() <= 1.0001);
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
					Vector3<T> u(x, y, z);

					// Do the math
					return T(2.0) * u.Dot(v) * u + (w*w - u.Dot(u)) * v + T(2.0) * w * u.Cross(v);
				}

				//ref : https://gamedev.stackexchange.com/questions/15070/orienting-a-model-to-face-a-target
				void OrientTo(const Vector3<T>& direction, const Vector3<T>& up = Vector3<T>::up())
				{
					auto old_direction = RotateVector(Vector3<T>::back());
					auto dot = old_direction.Dot(direction);

					if (std::abs(dot - (-1.0f)) < 0.000001f)
					{
						// direction and old_direction point exactly in the opposite direction, 
						// so it is a 180 degrees turn around the up-axis
						FromAxisAndAngle(Vector3<T>::up(), PI);
						return;
					}
					if (std::abs(dot - (1.0f)) < 0.000001f)
					{
						// direction and old_direction point exactly in the same direction
						// so we return the identity quaternion
						*this = Identity();
						return;
					}

					auto rotAngle = std::acos(dot);
					auto rotAxis = old_direction.Cross(direction);
					rotAxis.Normalize();
					FromAxisAndAngle(rotAxis, rotAngle);
				}

				//ref : Mathematics for 3D Game Programming And Computer Graphics
				void ToMatrix(Matrix4<T>& mat)const
				{
					mat.SetZero();
					mat.SetCell(3, 3, 1);
					
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
					mat.SetCell(0, 1, xy - wz);
					mat.SetCell(0, 2, xz + wy);

					mat.SetCell(1, 0, xy + wz);
					mat.SetCell(1, 1, 1 - x2 - z2);
					mat.SetCell(1, 2, yz - wx);

					mat.SetCell(2, 0, xz - wy);
					mat.SetCell(2, 1, yz + wx);
					mat.SetCell(2, 2, 1 - x2 - y2);
				}

				static const Quaternion& Identity()
				{
					static Quaternion quat;
					return quat;
				}
				T x, y, z, w;
			};

			using EulerAnglef = EulerAngle<float>;
			using Quaternionf = Quaternion<float>;
		}
	}
}
