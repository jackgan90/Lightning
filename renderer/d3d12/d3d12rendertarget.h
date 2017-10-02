#pragma once
#include "irendertarget.h"
#include<d3d12.h>
#include <wrl\client.h>

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12RenderTarget : public IRenderTarget
		{
		public:
			D3D12RenderTarget(ComPtr<ID3D12Resource> pRenderTarget);
		private:
			ComPtr<ID3D12Resource> m_nativeRenderTarget;
		};
	}
}