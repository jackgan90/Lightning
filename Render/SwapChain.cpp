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

		void SwapChain::OnFrameBegin()
		{
			
		}
	}
}