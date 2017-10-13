#include "rendererfactory.h"
#ifdef LIGHTNINGGE_USE_D3D12
#include "d3d12renderer.h"
#endif

namespace LightningGE
{
	namespace Renderer
	{
		UniqueRendererPtr RendererFactory::CreateRenderer(const WindowPtr& pWindow, const SharedFileSystemPtr& fs)const
		{
#ifdef LIGHTNINGGE_USE_D3D12
			//return std::make_shared<D3D12Renderer>(pWindow);
			return std::unique_ptr<D3D12Renderer>(new D3D12Renderer(pWindow, fs));
#else
			return nullptr;
#endif
		}
	}
}