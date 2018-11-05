#pragma once
#include <string>
#include <Windows.h>
#include "windowsystem.h"
#include "windowcomponent.h"

namespace Lightning
{
	namespace Window
	{
		using Foundation::EntityPtr;
		using Foundation::Entity;
		class LIGHTNING_WINDOW_API WinWindowSystem : public WindowSystem
		{
		public:
			WinWindowSystem();
			~WinWindowSystem()override;
			bool Show(bool show)override;
			void Update(const EntityPtr& entity)override;
			HWND GetWindowHandle()const { return mHwnd; }
		private:
			static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			void RegisterWindowClass(HINSTANCE hInstance);
			void CreateWindowInternal(HINSTANCE	hInstance);
			static constexpr char* WinWindowSystem::sWindowClassName = "DefaultWin32Window";
			HWND mHwnd;
		};
	}
}
