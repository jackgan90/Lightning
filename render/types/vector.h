#pragma once
#include <Eigen/StdVector>

template<typename T>
using EigenVector = std::vector<T, Eigen::aligned_allocator<T>>;

namespace LightningGE
{
	namespace Render
	{
		template<typename T, const int Dimension>
		class Vector
		{
		
		private:
			EigenVector m_value;
		};
	}
}