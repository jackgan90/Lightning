#pragma once
#include <Eigen/Dense>

namespace LightningGE
{
	namespace Renderer
	{
		template<typename T>
		struct Rect
		{
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			Rect():value(0){}
			Rect(T left, T top, T width, T height):value(left, top, width, height){}
			Rect(const T* dimension):value(dimension[0], dimension[1], dimension[2], dimension[3]){}
			Eigen::Matrix<T, 4, 1> value;
			const T left()const { return value[0]; }
			const T top()const { return value[1]; }
			const T width()const { return value[2]; }
			const T height()const { return value[3]; }
			const T right()const { return value[0] + value[2]; }
			const T bottom()const { return value[1] + value[3]; }
		};

		using RectI = Rect<int>;
		using RectF = Rect<float>;
	}
}