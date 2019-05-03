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
			void Start()override;
			void OnWindowMouseWheel(IWindow* window, int delta, bool isVertical)override;
			void OnWindowKeyDown(IWindow* window, VirtualKeyCode keyCode)override;
			void OnWindowMouseDown(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;
			void OnWindowMouseMove(IWindow* window, VirtualKeyCode keyCode, std::size_t x, std::size_t y)override;
		private:
			Scene::ISceneManager* mSceneMgr;
		};
	}
}
