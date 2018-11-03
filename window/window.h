#pragma once
#include "container.h"
#include "iwindow.h"

namespace Lightning
{
	namespace WindowSystem
	{
		using Foundation::container;
		class LIGHTNING_WINDOW_API Window : public IWindow
		{
		protected:
			virtual void OnIdle();
		};
	}
}
