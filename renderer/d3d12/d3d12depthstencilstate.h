#pragma once
#include <d3d12.h>
#include "idepthstencilstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12DepthStencilState : public IDepthStencilState
		{
		public:
			friend class D3D12PipelineStateObject;
			D3D12DepthStencilState();
			const DepthStencilConfiguration& GetConfiguration()override;
			bool SetConfiguration(const DepthStencilConfiguration& configuration)override;
			bool EnableDepthTest(bool enable)override;
			bool EnableStenciltest(bool enable)override;
			bool EnableDepthStencilTest(bool enable)override;
			bool SetStencilRef(const unsigned char ref)override;
		private:
			void InternalSetD3DDesc();
			DepthStencilConfiguration m_config;
			D3D12_DEPTH_STENCIL_DESC m_desc;
		};
	}
}