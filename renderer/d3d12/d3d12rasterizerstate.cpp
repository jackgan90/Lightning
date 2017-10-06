#include "d3d12typemapper.h"
#include "d3d12rasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RasterizerState::D3D12RasterizerState() :m_config(), m_desc{}
		{
			InternalSyncConfig();
		}

		const RasterizerStateConfiguration& D3D12RasterizerState::GetConfiguration()
		{
			return m_config;
		}

		bool D3D12RasterizerState::SetConfiguration(const RasterizerStateConfiguration& config)
		{
			m_config = config;
			InternalSyncConfig();
			return true;
		}

		bool D3D12RasterizerState::SetFillMode(FillMode mode)
		{
			m_config.fillMode = mode;
			m_desc.FillMode = D3D12TypeMapper::MapFillMode(m_config.fillMode);
			return true;
		}

		FillMode D3D12RasterizerState::GetFillMode()const
		{
			return m_config.fillMode;
		}

		bool D3D12RasterizerState::SetCullMode(CullMode mode)
		{
			m_config.cullMode = mode;
			m_desc.CullMode = D3D12TypeMapper::MapCullMode(m_config.cullMode);
			return true;
		}

		CullMode D3D12RasterizerState::GetCullMode()const
		{
			return m_config.cullMode;
		}

		bool D3D12RasterizerState::SetFrontFaceWindingOrder(FrontFaceWindingOrder order)
		{
			m_config.frontFaceWindingOrder = order;
			m_desc.FrontCounterClockwise = m_config.frontFaceWindingOrder == COUNTER_CLOCKWISE;
			return true;
		}

		FrontFaceWindingOrder D3D12RasterizerState::GetFrontFaceWindingOrder()const
		{
			return m_config.frontFaceWindingOrder;
		}

		void D3D12RasterizerState::InternalSyncConfig()
		{
			m_desc.FillMode = D3D12TypeMapper::MapFillMode(m_config.fillMode);
			m_desc.CullMode = D3D12TypeMapper::MapCullMode(m_config.cullMode);
			m_desc.FrontCounterClockwise = m_config.frontFaceWindingOrder == COUNTER_CLOCKWISE;
		}

	}
}