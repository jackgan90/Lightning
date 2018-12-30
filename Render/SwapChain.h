#pragma once
#include "ISwapChain.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Render
	{
		class SwapChain : public ISwapChain
		{
		public:
			SwapChain(Window::IWindow* outputWindow);
			~SwapChain();
			virtual void OnFrameBegin();
		protected:
			IRenderTarget* mRenderTargets[RENDER_FRAME_COUNT];
			Window::IWindow* mOutputWindow;
		};
	}
}