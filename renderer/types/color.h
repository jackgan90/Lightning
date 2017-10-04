#pragma once
#include <Eigen/Dense>

namespace LightningGE
{
	namespace Renderer
	{
		template<typename T>
		struct Color
		{
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Color():value(0, 0, 0, 0){}
			Color(T r, T g, T b, T a):value(r, g, b, a){}
			Color(const T* components):value(components[0], components[1], components[2], components[3]){}
			Eigen::Matrix<T, 4, 1> value;
			const T r()const { return value[0]; }
			const T g()const { return value[1]; }
			const T b()const { return value[2]; }
			const T a()const { return value[3]; }
		};

		using ColorF = Color<float>;
		using Color32 = Color<unsigned int>;
	}
}