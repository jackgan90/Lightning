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
		protected:
			void OnMouseWheel(const Foundation::IEvent& event);
			void OnKeyDown(const Foundation::IEvent& event);
			void OnMouseDown(const Foundation::IEvent& event);
			void OnMouseMove(const Foundation::IEvent& event);
			void RegisterWindowHandlers()override;
		private:
			Scene::ISceneManager* mSceneMgr;
		};
	}
}
