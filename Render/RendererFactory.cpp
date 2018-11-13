#include "RendererFactory.h"
#ifdef LIGHTNING_USE_D3D12
#include "D3D12Renderer.h"
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
