#pragma once
#include <cmath>
#include <limits>
#include <functional>
#include "plainobject.h"

namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
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
						if (std::abs(pDerived->operator[](i)) > std::numeric_limits<T>::epsilon())
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

			template<typename T>
			struct Vector2 : VectorBase<Vector2<T>, T>
			{
				static constexpr unsigned Order = 2;
				Vector2() : x(0), y(0){}
				Vector2(T _x, T _y) : x(_x), y(_y){}
				T& operator[](int i) {
					switch (i) {
					case 1:
						return y;
					default:
						return x;
					}
				}

				T operator[](int i)const {
					switch (i) {
					case 1:
						return y;
					default:
						return x;
					}
				}

				T x, y;
			};

			template<typename T>
			struct Vector4;

			template<typename T>
			struct Vector3 : VectorBase<Vector3<T>, T>
			{
				static constexpr unsigned Order = 3;
				Vector3() : x(0), y(0), z(0){}
				Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z){}
				Vector3(const Vector4<T>& v4) : x(v4.x), y(v4.y), z(v4.z){}
				T& operator[](int i) {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					default:
						return x;
					}
				}

				T operator[](int i)const {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					default:
						return x;
					}
				}

				Vector3<T> Cross(const Vector3<T>& other)const
				{
					return Vector3<T>(y * other.z - z * other.y,
						z * other.x - x * other.z,
						x * other.y - y * other.x);
				}
				T x, y, z;
				//Don't make up/down/right/left/forward/back static member which may cause 
				//issues related to static member initialization order
				static const Vector3<T> up()
				{
					static Vector3<T> inst_up(0, 1, 0);
					return inst_up;
				};
				static const Vector3<T> down()
				{
					static Vector3<T> inst_down(0, -1, 0);
					return inst_down;
				}
				static const Vector3<T> right()
				{
					static Vector3<T> inst_right(1, 0, 0);
					return inst_right;
				}
				static const Vector3<T> left()
				{
					static Vector3<T> inst_left(-1, 0, 0);
					return inst_left;
				}
				static const Vector3<T> forward()
				{
					static Vector3<T> inst_forward(0, 0, 1);
					return inst_forward;
				}
				static const Vector3<T> back()
				{
					static Vector3<T> inst_back(0, 0, -1);
					return inst_back;
				}
			};

			template<typename T>
			struct Vector4 : VectorBase<Vector4<T>, T>
			{
				static constexpr unsigned Order = 4;
				Vector4() : x(0), y(0), z(0), w(0){}
				Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w){}
				Vector4(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z), w(1){}
				T& operator[](int i) {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					case 3:
						return w;
					default:
						return x;
					}
				}

				T operator[](int i)const {
					switch (i) {
					case 1:
						return y;
					case 2:
						return z;
					case 3:
						return w;
					default:
						return x;
					}
				}
				T x, y, z, w;
			};

			template<typename V, typename T>
			std::enable_if_t<std::is_base_of<VectorBase<V, T>, V>::value, void> 
			VectorMulScalar(const V& v, const T& scalar, V& res)
			{
				for (int i = 0;i < V::Order;++i)
				{
					res[i] = v[i] * scalar;
				}
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
			using Vector3f = Vector3<float>;
			using Vector2f = Vector2<float>;
			using Vector4i = Vector4<int>;
			using Vector3i = Vector3<int>;
			using Vector2i = Vector2<int>;
		}
	}
}