#include "d3d12typemapper.h"
#include "d3d12rasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RasterizerState::D3D12RasterizerState() :RasterizerState(),m_desc{}
		{
			InternalSyncConfig();
		}


		bool D3D12RasterizerState::SetConfiguration(const RasterizerStateConfiguration& config)
		{
			if (!RasterizerState::SetConfiguration(config))
				return false;
			InternalSyncConfig();
			return true;
		}

		bool D3D12RasterizerState::SetFillMode(FillMode mode)
		{
			if (!RasterizerState::SetFillMode(mode))
				return false;
			m_desc.FillMode = D3D12TypeMapper::MapFillMode(m_config.fillMode);
			return true;
		}


		bool D3D12RasterizerState::SetCullMode(CullMode mode)
		{
			if (!RasterizerState::SetCullMode(mode))
				return false;
			m_desc.CullMode = D3D12TypeMapper::MapCullMode(m_config.cullMode);
			return true;
		}

		bool D3D12RasterizerState::SetFrontFaceWindingOrder(FrontFaceWindingOrder order)
		{
			if (!RasterizerState::SetFrontFaceWindingOrder(order))
				return false;
			m_desc.FrontCounterClockwise = m_config.frontFaceWindingOrder == COUNTER_CLOCKWISE;
			return true;
		}

		void D3D12RasterizerState::InternalSyncConfig()
		{
			m_desc.FillMode = D3D12TypeMapper::MapFillMode(m_config.fillMode);
			m_desc.CullMode = D3D12TypeMapper::MapCullMode(m_config.cullMode);
			m_desc.FrontCounterClockwise = m_config.frontFaceWindingOrder == COUNTER_CLOCKWISE;
		}

	}
}