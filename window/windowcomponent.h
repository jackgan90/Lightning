#pragma once
#include <string>
#include "ecs/component.h"

namespace Lightning
{
	namespace Window1
	{
		using Foundation::Component;
		struct WindowComponent : Component<WindowComponent>
		{
			std::string caption;
			std::uint32_t width;
			std::uint32_t height;
		};
	}
}