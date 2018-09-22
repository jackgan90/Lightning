#pragma once
namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			constexpr double PI = 3.141592653589793;
			template<typename T>
			inline float DegreesToRadians(T degree)
			{
				return static_cast<float>(degree * PI / 180.0);
			}
			template<typename T>
			inline float RadiansToDegrees(T radians)
			{
				return static_cast<float>(radians * 180.0 / PI);
			}
		}
	}
}