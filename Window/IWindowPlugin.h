#pragma once
#include "IPlugin.h"
#include "IWindow.h"
#undef CreateWindow

namespace Lightning
{
	namespace Plugins
	{
		struct IWindowPlugin : public IPlugin
		{
			virtual Window::IWindow* INTERFACECALL CreateWindow() = 0;
		};
	}
}