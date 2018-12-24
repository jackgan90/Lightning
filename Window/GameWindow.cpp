#include <algorithm>
#include "GameWindow.h"
#include "IPluginManager.h"
#include "IFoundationPlugin.h"

#define DISPATCH_WINDOW_EVENT(method, ...)\
for (auto receiver : mEventReceivers)\
{\
	receiver->##method(this, __VA_ARGS__);\
}

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginManager* gPluginMgr;
	}
	namespace Window
	{
		GameWindow::GameWindow()
		{
		}

		bool GameWindow::RegisterEventReceiver(IWindowEventReceiver* receiver)
		{
			if (!receiver)
				return false;
			for (auto r : mEventReceivers)
			{
				if (r == receiver)
					return false;
			}
			mEventReceivers.push_back(receiver);
			return true;
		}

		bool GameWindow::UnregisterEventReceiver(IWindowEventReceiver* receiver)
		{
			if (!receiver)
				return false;
			auto it = std::find(mEventReceivers.cbegin(), mEventReceivers.cend(), receiver);
			if (it == mEventReceivers.end())
			{
				return false;
			}
			mEventReceivers.erase(it);
			return true;
		}

		void GameWindow::OnCreated()
		{
			DISPATCH_WINDOW_EVENT(OnWindowCreated)
		}

		void GameWindow::OnIdle()
		{
			DISPATCH_WINDOW_EVENT(OnWindowIdle)
		}

		void GameWindow::OnResize(std::size_t width, std::size_t height)
		{
			DISPATCH_WINDOW_EVENT(OnWindowResize, width, height)
		}

		void GameWindow::OnMouseWheel(int delta, bool isVertical)
		{
			DISPATCH_WINDOW_EVENT(OnWindowMouseWheel, delta, isVertical)
		}

		void GameWindow::OnKeyDown(VirtualKeyCode keyCode)
		{
			DISPATCH_WINDOW_EVENT(OnWindowKeyDown, keyCode)
		}

		void GameWindow::OnMouseDown(std::size_t x, std::size_t y, VirtualKeyCode keyCode)
		{
			DISPATCH_WINDOW_EVENT(OnWindowMouseDown, keyCode, x, y)
		}

		void GameWindow::OnMouseMove(std::size_t x, std::size_t y, VirtualKeyCode keyCode)
		{
			DISPATCH_WINDOW_EVENT(OnWindowMouseMove, keyCode, x, y)
		}

		void GameWindow::OnDestroy(int exitCode)
		{
			DISPATCH_WINDOW_EVENT(OnWindowDestroy, exitCode)
		}
	}
}