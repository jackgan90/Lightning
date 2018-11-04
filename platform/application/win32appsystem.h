#pragma once
#include "platformexportdef.h"
#include "appsystem.h"
#include "windowsystem.h"
#include "filesystem.h"

namespace Lightning
{
	namespace App
	{
		using Window1::SharedWindowPtr;
		class LIGHTNING_PLATFORM_API Win32AppSystem : public AppSystem
		{
		public:
			Win32AppSystem();
			~Win32AppSystem()override;
			void Update(const EntityPtr<Entity>& entity)override;
		protected:
			void OnMouseWheel(const Window1::MouseWheelEvent& event);
			void OnKeyDown(const Window1::KeyEvent& event);
			void OnMouseDown(const Window1::MouseDownEvent& event);
			void OnMouseMove(const Window1::MouseMoveEvent& event);
			void RegisterWindowHandlers()override;
			SharedWindowPtr CreateMainWindow()override;
			UniqueRendererPtr CreateRenderer()override;
		};
	}
}
