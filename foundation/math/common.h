#pragma once
namespace Lightning
{
	namespace Foundation
	{
		namespace Math
		{
			constexpr double PI = 3.141592653589793;
			inline float DegreesToRadians(const float degree)
			{
				return static_cast<float>(degree * PI / 180.0);
			}
			inline float RadiansToDegrees(const float radians)
			{
				return static_cast<float>(radians * 180.0 / PI);
			}
		}
	}
}