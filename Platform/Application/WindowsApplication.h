#pragma once
#include "Application.h"
#include "IFileSystem.h"
#include "ISceneManager.h"

namespace Lightning
{
	namespace Render
	{
		class IRenderer;
	}

	namespace App
	{
		class WindowsApplication : public Application
		{
		public:
			void Start()override;
		protected:
			void OnMouseWheel(const Window::MouseWheelEvent& event);
			void OnKeyDown(const Window::KeyEvent& event);
			void OnMouseDown(const Window::MouseDownEvent& event);
			void OnMouseMove(const Window::MouseMoveEvent& event);
			void RegisterWindowHandlers()override;
		private:
			Scene::ISceneManager* mSceneMgr;
		};
	}
}
