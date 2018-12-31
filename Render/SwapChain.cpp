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
			std::memset(mRenderTargets, 0, sizeof(mRenderTargets));
			mOutputWindow->AddRef();
		}

		SwapChain::~SwapChain()
		{
			mOutputWindow->Release();
			for (auto renderTarget : mRenderTargets)
			{
				if (renderTarget)
				{
					renderTarget->Release();
				}
			}
		}

		bool SwapChain::CheckIfBackBufferNeedsResize()
		{
			auto width = mOutputWindow->GetWidth();
			auto height = mOutputWindow->GetHeight();
			auto bufferIndex = GetCurrentBackBufferIndex();
			auto renderTarget = mRenderTargets[bufferIndex];
			if (!renderTarget)
				return false;
			auto renderTexture = renderTarget->GetTexture();
			if (!renderTexture)
				return false;
			if (width != renderTexture->GetWidth() || height != renderTexture->GetHeight())
			{
				return true;
			}
			return false;
		}
	}
}