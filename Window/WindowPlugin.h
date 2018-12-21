#pragma once
#include "IPlugin.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Plugins
	{
		struct WindowPlugin : public IPlugin
		{
			virtual Window::IWindow* NewWindow() = 0;
		};
	}
}