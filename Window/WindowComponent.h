#pragma once
#include <string>
#include "ECS/Component.h"

namespace Lightning
{
	namespace Window
	{
		using Foundation::Component;
		struct WindowComponent : Component
		{
			std::string caption;
			std::uint32_t width;
			std::uint32_t height;
			RTTR_ENABLE(Component)
		};
	}
}