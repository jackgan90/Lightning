#pragma once
#include "IPlugin.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Plugins
	{
		struct IWindowPlugin : public IPlugin
		{
			virtual Window::IWindow* INTERFACECALL NewWindow() = 0;
		};
	}
}