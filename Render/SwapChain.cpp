#include <cassert>
#include "SwapChain.h"

namespace Lightning
{
	namespace Render
	{
		SwapChain::SwapChain(Window::IWindow* outputWindow)
			:mOutputWindow(outputWindow)
		{
			assert(mOutputWindow != nullptr && "Output window must not be nullptr.");
		}

		SwapChain::~SwapChain()
		{
		}

		bool SwapChain::CheckIfBackBufferNeedsResize()
		{
			auto renderTarget = GetCurrentRenderTarget();
			if (!renderTarget)
				return false;
			auto renderTexture = renderTarget->GetTexture();
			if (!renderTexture)
				return false;
			auto width = mOutputWindow->GetWidth();
			auto height = mOutputWindow->GetHeight();
			if (width != renderTexture->GetWidth() || height != renderTexture->GetHeight())
			{
				return true;
			}
			return false;
		}
	}
}