#pragma once
#include <type_traits>
#include <cmath>
#include <limits>
#include <functional>
#include "PlainObject.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			//Vector2
			template<typename T>
			struct Vector2
			{
				static_assert(std::is_pod<T>::value, "T is not a POD type!");
				static Vector2<T>& Zero()
				{
					static Vector2<T> d;
					d.x = d.y = T(0);
					return d;
				}

				T Dot(const Vector2<T>& other)const
				{
					return x * other.x + y * other.y;
				}

				Vector2<T> operator-()const
				{
					Vector2<T> res;
					res.x = -x;
					res.y = -y;
					return res;
				}

				Vector2<T> operator-(const Vector2<T>& other)const
				{
					Vector2<T> res;
					res.x = x - other.x;
					res.y = y - other.y;
					return res;
				}

				Vector2<T> operator+(const Vector2<T>& other)const
				{
					Vector2<T> res;
					res.x = x + other.x;
					res.y = y + other.y;
					return res;
				}

				Vector2<T>& operator+=(const Vector2<T>& other)
				{
					x += other.x;
					y += other.y;
					return *this;
				}

				Vector2<T>& operator*=(T scalar)
				{
					x *= scalar;
					y *= scalar;
					return *this;
				}

				bool operator==(const Vector2<T>& other)
				{
					return x == other.x && y == other.y;
				}

				bool operator!=(const Vector2<T>& other)
				{
					return !operator==(other);
				}

				bool IsZero()const
				{
					return std::abs(x) <= T(0.000001) && std::abs(y) <= T(0.000001);
				}

				T SquareLength()const
				{
					return x * x + y * y;
				}

				T Length()const
				{
					return std::sqrt(SquareLength());
				}

				bool IsUnitVector()const
				{
					auto l = SquareLength();
					return l >= 0.99999 && l <= 1.00001
				}

				void Normalize()
				{
					auto l = Length();
					x /= l;
					y /= l;
				}

				Vector2<T> Normalized()const
				{
					Vector2<T> res;
					auto l = Length();
					res.x = x / l;
					res.y = y / l;
					return res;
				}

				T x, y;
			};

			template<typename T>
			Vector2<T> operator*(const Vector2<T>& v, T scalar)
			{
				Vector2<T> res;
				res.x = v.x * scalar;
				res.y = v.y * scalar;
				return res;
			}

			template<typename T>
			Vector2<T> operator*(T scalar, const Vector2<T>& v)
			{
				return operator*(v, scalar);
			}

			using Vector2f = Vector2<float>;
			using Vector2i = Vector2<int>;
			static_assert(std::is_pod<Vector2f>::value, "Vector2f is not POD!");
			static_assert(std::is_pod<Vector2i>::value, "Vector2i is not POD!");

			//Vector3
			template<typename T>
			struct Vector3
			{
				static_assert(std::is_pod<T>::value, "T is not a POD type!");
				//Don't make up/down/right/left/forward/back static member which may cause 
				//issues related to static member initialization order
				static Vector3<T>& Zero()
				{
					static Vector3<T> d;
					d.x = d.y = d.z = T(0);
					return d;
				}
				static const Vector3<T> up()
				{
					static Vector3<T> inst_up;
					inst_up.x = T(0);
					inst_up.y = T(1);
					inst_up.z = T(0);
					return inst_up;
				};
				static const Vector3<T> down()
				{
					static Vector3<T> inst_down;
					inst_down.x = T(0);
					inst_down.y = T(-1);
					inst_down.z = T(0);
					return inst_down;
				}
				static const Vector3<T> right()
				{
					static Vector3<T> inst_right;
					inst_right.x = T(1);
					inst_right.y = T(0);
					inst_right.z = T(0);
					return inst_right;
				}
				static const Vector3<T> left()
				{
					static Vector3<T> inst_left;
					inst_left.x = T(-1);
					inst_left.y = T(0);
					inst_left.z = T(0);
					return inst_left;
				}
				static const Vector3<T> forward()
				{
					static Vector3<T> inst_forward;
					inst_forward.x = T(0);
					inst_forward.y = T(0);
					inst_forward.z = T(1);
					return inst_forward;
				}
				static const Vector3<T> back()
				{
					static Vector3<T> inst_back;
					inst_back.x = T(0);
					inst_back.y = T(0);
					inst_back.z = T(-1);
					return inst_back;
				}

				T Dot(const Vector3<T>& other)const
				{
					return x * other.x + y * other.y + z * other.z;
				}

				Vector3<T> Cross(const Vector3<T>& other)const
				{
					Vector3<T> res;
					res.x = y * other.z - z * other.y;
					res.y = z * other.x - x * other.z;
					res.z = x * other.y - y * other.x;
					return res;
				}

				Vector3<T> operator-()const
				{
					Vector3<T> res;
					res.x = -x;
					res.y = -y;
					res.z = -z;
					return res;
				}

				Vector3<T> operator-(const Vector3<T>& other)const
				{
					Vector3<T> res;
					res.x = x - other.x;
					res.y = y - other.y;
					res.z = z - other.z;
					return res;
				}

				Vector3<T> operator+(const Vector3<T>& other)const
				{
					Vector3<T> res;
					res.x = x + other.x;
					res.y = y + other.y;
					res.z = z + other.z;
					return res;
				}

				Vector3<T>& operator+=(const Vector3<T>& other)
				{
					x += other.x;
					y += other.y;
					z += other.z;
					return *this;
				}

				Vector3<T>& operator*=(T scalar)
				{
					x *= scalar;
					y *= scalar;
					z *= scalar;
					return *this;
				}

				bool operator==(const Vector3<T>& other)
				{
					return x == other.x && y == other.y && z == other.z;
				}

				bool operator!=(const Vector3<T>& other)
				{
					return !operator==(other);
				}

				bool IsZero()const
				{
					return std::abs(x) <= T(0.000001) && std::abs(y) <= T(0.000001) && std::abs(z) <= T(0.000001);
				}

				T SquareLength()const
				{
					return x * x + y * y + z * z;
				}

				T Length()const
				{
					return std::sqrt(SquareLength());
				}

				bool IsUnitVector()const
				{
					auto l = SquareLength();
					return l >= 0.99999 && l <= 1.00001
				}

				void Normalize()
				{
					auto l = Length();
					x /= l;
					y /= l;
					z /= l;
				}

				Vector3<T> Normalized()const
				{
					Vector3<T> res;
					auto l = Length();
					res.x = x / l;
					res.y = y / l;
					res.z = z / l;
					return res;
				}

				T x, y, z;
			};

			template<typename T>
			Vector3<T> operator*(const Vector3<T>& v, T scalar)
			{
				Vector3<T> res;
				res.x = v.x * scalar;
				res.y = v.y * scalar;
				res.z = v.z * scalar;
				return res;
			}

			template<typename T>
			Vector3<T> operator*(T scalar, const Vector3<T>& v)
			{
				return operator*(v, scalar);
			}

			using Vector3f = Vector3<float>;
			using Vector3i = Vector3<int>;
			static_assert(std::is_pod<Vector3f>::value, "Vector3f is not POD!");
			static_assert(std::is_pod<Vector3i>::value, "Vector3i is not POD!");
			
			//Vector4
			template<typename T>
			struct Vector4
			{
				static_assert(std::is_pod<T>::value, "T is not a POD type!");
				static Vector4<T>& Zero()
				{
					static Vector4<T> d;
					d.x = d.y = d.z = d.w = T(0);
					return d;
				}

				T Dot(const Vector4<T>& other)const
				{
					return x * other.x + y * other.y + z * other.z + w * other.w;
				}

				Vector4<T> operator-()const
				{
					Vector4<T> res;
					res.x = -x;
					res.y = -y;
					res.z = -z;
					res.w = -w;
					return res;
				}

				Vector4<T> operator-(const Vector4<T>& other)const
				{
					Vector4<T> res;
					res.x = x - other.x;
					res.y = y - other.y;
					res.z = z - other.z;
					res.w = w - other.w;
					return res;
				}

				Vector4<T> operator+(const Vector4<T>& other)const
				{
					Vector4<T> res;
					res.x = x + other.x;
					res.y = y + other.y;
					res.z = z + other.z;
					res.w = w + other.w;
					return res;
				}

				Vector4<T>& operator+=(const Vector4<T>& other)
				{
					x += other.x;
					y += other.y;
					z += other.z;
					w += other.w;
					return *this;
				}

				Vector4<T>& operator*=(T scalar)
				{
					x *= scalar;
					y *= scalar;
					z *= scalar;
					w *= scalar;
					return *this;
				}

				bool operator==(const Vector4<T>& other)
				{
					return x == other.x && y == other.y && z == other.z && w == other.w;
				}

				bool operator!=(const Vector4<T>& other)
				{
					return !operator==(other);
				}

				bool IsZero()const
				{
					return std::abs(x) <= T(0.000001) && std::abs(y) <= T(0.000001) 
						&& std::abs(z) <= T(0.000001) && std::abs(w) <= T(0.000001);
				}

				T SquareLength()const
				{
					return x * x + y * y + z * z + w * w;
				}

				T Length()const
				{
					return std::sqrt(SquareLength());
				}

				bool IsUnitVector()const
				{
					auto l = SquareLength();
					return l >= 0.99999 && l <= 1.00001
				}

				void Normalize()
				{
					auto l = Length();
					x /= l;
					y /= l;
					z /= l;
					w /= l;
				}

				Vector4<T> Normalized()const
				{
					Vector4<T> res;
					auto l = Length();
					res.x = x / l;
					res.y = y / l;
					res.z = z / l;
					res.w = w / l;
					return res;
				}

				T x, y, z, w;
			};

			template<typename T>
			Vector4<T> operator*(const Vector4<T>& v, T scalar)
			{
				Vector4<T> res;
				res.x = v.x * scalar;
				res.y = v.y * scalar;
				res.z = v.z * scalar;
				res.w = v.w * scalar;
				return res;
			}

			template<typename T>
			Vector4<T> operator*(T scalar, const Vector4<T>& v)
			{
				return operator*(v, scalar);
			}
			

			using Vector4f = Vector4<float>;
			using Vector4i = Vector4<int>;
			static_assert(std::is_pod<Vector4f>::value, "Vector4f is not POD!");
			static_assert(std::is_pod<Vector4i>::value, "Vector4i is not POD!");

			/*
			template<typename Derived, typename T>
			struct VectorBase : PlainObject<Derived>
			{
				static Derived& Zero()
				{
					static Derived d;
					return d;
				}

				T Dot(const Derived& other)const
				{
					T res{ 0 };
					const Derived *const pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						res += pDerived->operator[](i) * other[i];
					}
					return res;
				}
				Derived operator-()const
				{
					Derived res;
					const Derived *const pDerived = reinterpret_cast<const Derived*const>(this);
					for (int i = 0;i < Derived::Order;++i)
					{
						res[i] = -pDerived->operator[](i);
					}
					return res;
				}

				Derived operator-(const Derived& other)const
				{
					return ComponentwiseOperation(other, [](T comp0, T comp1) {return comp0 - comp1; });
				}

				Derived operator+(const Derived& other)const
				{
					return ComponentwiseOperation(other, [](T comp0, T comp1) {return comp0 + comp1; });
				}

				Derived ComponentwiseOperation(const Derived& other, std::function<T(T,T)> func)const
				{
					Derived res;
					const Derived *pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
					{
						res[i] = func(pDerived->operator[](i), other[i]);
					}
					return res;
				}

				bool IsZero()const
				{
					const Derived *pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
					{
						if (std::abs(pDerived->operator[](i)) > 0.00001)
							return false;
					}
					return true;
				}

				bool IsUnitVector()const
				{
					auto sqrl = SquareLength();
					return sqrl >= 0.9999 && sqrl <= 1.0001;
				}

				T SquareLength()const
				{
					T sqrLength{ 0 };
					const Derived *pDerived = reinterpret_cast<const Derived* const>(this);
					for (int i = 0; i < Derived::Order; ++i)
					{
						sqrLength += pDerived->operator[](i) * pDerived->operator[](i);
					}
					return sqrLength;
				}

				T Length()const
				{
					return std::sqrt(SquareLength());
				}

				void Normalize()
				{
					auto length = Length();
					Derived* const pDerived = reinterpret_cast<Derived* const>(this);
					if (length > 1e-5)
					{
						for (int i = 0;i < Derived::Order;++i)
						{
							auto& val = pDerived->operator[](i);
							val /= length;
						}
					}
				}

				Derived Normalized()const
				{
					const Derived *const pDerived = reinterpret_cast<const Derived *const>(this);
					Derived res(*pDerived);
					res.Normalize();
					return res;
				}

				Derived& operator+=(const Derived& other)
				{
					Derived* const pThis = static_cast<Derived*>(this);
					*pThis = *pThis + other;
					return *pThis;
				}

				Derived& operator*=(const T& scalar);
			};

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, void> 
			VectorMulScalar(const V& v, const T& scalar, V& res)
			{
				for (int i = 0;i < V::Order;++i)
				{
					res[i] = v[i] * scalar;
				}T(0.000001)
			}

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, V> operator*(const V& v, const T& scalar)
			{
				V res;
				VectorMulScalar(v, scalar, res);
				return res;
			}

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, V> operator*(const T& scalar, const V& v)
			{
				V res;
				VectorMulScalar(v, scalar, res);
				return res;
			}

			template<typename Derived, typename T>
			Derived& VectorBase<Derived, T>::operator*=(const T& scalar)
			{
				VectorMulScalar(static_cast<const Derived&>(*this), scalar, static_cast<Derived&>(*this));
				return static_cast<Derived&>(*this);
			}
			

			using Vector4f = Vector4<float>;
			using Vector4i = Vector4<int>;
			*/
		}
	}
}