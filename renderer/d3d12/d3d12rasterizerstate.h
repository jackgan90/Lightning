#pragma once
#include <d3d12.h>
#include "irasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		class D3D12RasterizerState : public IRasterizerState
		{
		public:
			friend class D3D12PipelineStateObject;
			D3D12RasterizerState();
			const RasterizerStateConfiguration& GetConfiguration()override;
			bool SetConfiguration(const RasterizerStateConfiguration& config)override;
			bool SetFillMode(FillMode mode)override;
			FillMode GetFillMode()const override;
			bool SetCullMode(CullMode mode)override;
			CullMode GetCullMode()const override;
			bool SetFrontFaceWindingOrder(FrontFaceWindingOrder order)override;
			FrontFaceWindingOrder GetFrontFaceWindingOrder()const override;
		private:
			void InternalSyncConfig();
			RasterizerStateConfiguration m_config;
			D3D12_RASTERIZER_DESC m_desc;
		};
	}
}