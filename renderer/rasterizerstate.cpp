#include <boost/functional/hash.hpp>
#include "irasterizerstate.h"

namespace LightningGE
{
	namespace Renderer
	{
		RasterizerState::RasterizerState() :m_config()
		{

		}

		const RasterizerStateConfiguration& RasterizerState::GetConfiguration()
		{
			return m_config;
		}

		bool RasterizerState::SetConfiguration(const RasterizerStateConfiguration& config)
		{
			m_config = config;
			SetHashDirty();
			return true;
		}

		bool RasterizerState::SetFillMode(FillMode mode)
		{
			if (m_config.fillMode != mode)
				SetHashDirty();
			m_config.fillMode = mode;
			return true;
		}

		FillMode RasterizerState::GetFillMode()const
		{
			return m_config.fillMode;
		}

		bool RasterizerState::SetCullMode(CullMode mode)
		{
			if(m_config.cullMode != mode)
				SetHashDirty();

			m_config.cullMode = mode;
			return true;
		}

		CullMode RasterizerState::GetCullMode()const
		{
			return m_config.cullMode;
		}

		bool RasterizerState::SetFrontFaceWindingOrder(FrontFaceWindingOrder order)
		{
			if(m_config.frontFaceWindingOrder != order)
				SetHashDirty();

			m_config.frontFaceWindingOrder = order;
			return true;

		}

		FrontFaceWindingOrder RasterizerState::GetFrontFaceWindingOrder()const
		{
			return m_config.frontFaceWindingOrder;
		}

		size_t RasterizerState::CalculateHashInternal()
		{
			size_t seed = 0;
			boost::hash_combine(seed, m_config.cullMode);
			boost::hash_combine(seed, m_config.fillMode);
			boost::hash_combine(seed, m_config.frontFaceWindingOrder);
			return seed;
		}
	}
}