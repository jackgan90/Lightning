#include "d3d12depthstencilstate.h"
#include "d3d12typemapper.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12DepthStencilState::D3D12DepthStencilState():DepthStencilState()
		{
			InternalSetD3DDesc();
		}

		bool D3D12DepthStencilState::SetConfiguration(const DepthStencilConfiguration& configuration)
		{
			if (!DepthStencilState::SetConfiguration(configuration))
				return false;
			InternalSetD3DDesc();
			return true;
		}

		void D3D12DepthStencilState::InternalSetD3DDesc()
		{
			m_desc.DepthEnable = m_config.depthTestEnable;
			m_desc.DepthFunc = D3D12TypeMapper::MapCmpFunc(m_config.depthCmpFunc);
			m_desc.DepthWriteMask = m_config.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			m_desc.StencilEnable = m_config.stencilEnable;
			m_desc.StencilReadMask = m_config.stencilReadMask;
			m_desc.StencilWriteMask = m_config.stencilWriteMask;

			m_desc.FrontFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(m_config.frontFace.cmpFunc);
			m_desc.FrontFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(m_config.frontFace.passOp);
			m_desc.FrontFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(m_config.frontFace.failOp);
			m_desc.FrontFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(m_config.frontFace.depthFailOp);

			m_desc.BackFace.StencilFunc = D3D12TypeMapper::MapCmpFunc(m_config.backFace.cmpFunc);
			m_desc.BackFace.StencilPassOp = D3D12TypeMapper::MapStencilOp(m_config.backFace.passOp);
			m_desc.BackFace.StencilFailOp = D3D12TypeMapper::MapStencilOp(m_config.backFace.failOp);
			m_desc.BackFace.StencilDepthFailOp = D3D12TypeMapper::MapStencilOp(m_config.backFace.depthFailOp);
		}


		bool D3D12DepthStencilState::EnableDepthTest(bool enable)
		{
			if (!DepthStencilState::EnableDepthTest(enable))
				return false;
			m_desc.DepthEnable = enable;
			return true;
		}

		bool D3D12DepthStencilState::EnableStenciltest(bool enable)
		{
			if (!DepthStencilState::EnableStenciltest(enable))
				return false;
			m_desc.StencilEnable = enable;
			return true;
		}

	}
}