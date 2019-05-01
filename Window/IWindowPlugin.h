#pragma once
#include "Plugin.h"
#include "IWindow.h"
#undef CreateWindow

namespace Lightning
{
	namespace Plugins
	{
		struct IWindowPlugin : public Plugin
		{
			virtual Window::IWindow* CreateWindow() = 0;
		};
	}
}