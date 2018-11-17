#include "RendererFactory.h"
#ifdef LIGHTNING_USE_D3D12
#include "D3D12Renderer.h"
#endif

namespace Lightning
{
	namespace Render
	{
		IRenderer* RendererFactory::CreateRenderer(Window::IWindow* window)const
		{
#ifdef LIGHTNING_USE_D3D12
			//return std::make_shared<D3D12Renderer>(pWindow);
			return new D3D12Renderer(window);
#else
			return nullptr;
#endif
		}
	}
}
