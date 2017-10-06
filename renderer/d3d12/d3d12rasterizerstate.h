#pragma once
#include <d3d12.h>
#include "irasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12RasterizerState : public RasterizerState
		{
		public:
			friend class D3D12PipelineStateObject;
			D3D12RasterizerState();
			bool SetConfiguration(const RasterizerStateConfiguration& config)override;
			bool SetFillMode(FillMode mode)override;
			bool SetCullMode(CullMode mode)override;
			bool SetFrontFaceWindingOrder(FrontFaceWindingOrder order)override;
		private:
			void InternalSyncConfig();
			D3D12_RASTERIZER_DESC m_desc;
		};
	}
}