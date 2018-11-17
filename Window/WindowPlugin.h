#pragma once
#include "Plugin.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Plugins
	{
		class WindowPlugin : public Plugin
		{
		public:
			virtual Window::IWindow* NewWindow() = 0;
		};
	}
}