#pragma once
#include <Windows.h>
#include "ecs/component.h"
#include "ecs/event.h"

namespace Lightning
{
	namespace Window1
	{
		using Foundation::Component;
		using Foundation::Event;

		struct WinWindowComponent : Component<WinWindowComponent>
		{
			HWND hwnd;
		};
	}
}