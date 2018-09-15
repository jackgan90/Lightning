#include "rendererfactory.h"
#ifdef LIGHTNING_USE_D3D12
#include "d3d12renderer.h"
#endif

namespace Lightning
{
	namespace Render
	{
		UniqueRendererPtr RendererFactory::CreateRenderer(const SharedWindowPtr& pWindow, const SharedFileSystemPtr& fs)const
		{
#ifdef LIGHTNING_USE_D3D12
			//return std::make_shared<D3D12Renderer>(pWindow);
			return std::unique_ptr<D3D12Renderer>(new D3D12Renderer(pWindow, fs));
#else
			return nullptr;
#endif
		}
	}
}
