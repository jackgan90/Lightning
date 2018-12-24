#pragma once
#include "Application.h"
#include "IFileSystem.h"
#include "ISceneManager.h"

namespace Lightning
{
	namespace Render
	{
		struct IRenderer;
	}

	namespace App
	{
		class WindowsApplication : public Application
		{
		public:
			void INTERFACECALL Start()override;
			void INTERFACECALL OnWindowMouseWheel(IWindow* window, int delta, bool isVertical)override;
			void INTERFACECALL OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode)override;
			void INTERFACECALL OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;
			void INTERFACECALL OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;
		private:
			Scene::ISceneManager* mSceneMgr;
		};
	}
}
