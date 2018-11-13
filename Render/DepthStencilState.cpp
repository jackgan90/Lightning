#include <boost/functional/hash.hpp>
#include "idepthstencilstate.h"

namespace Lightning
{
	namespace Renderer
	{
		const DepthStencilConfiguration& DepthStencilState::GetConfiguration()
		{
			return m_config;
		}

		bool DepthStencilState::SetConfiguration(const DepthStencilConfiguration& configuration)
		{
			m_config = configuration;
			SetHashDirty();
			return true;
		}

		bool DepthStencilState::EnableDepthTest(bool enable)
		{
			if(m_config.depthTestEnable != enable)
				SetHashDirty();

			m_config.depthTestEnable = enable;
			return true;
		}

		bool DepthStencilState::EnableStenciltest(bool enable)
		{
			if(m_config.stencilEnable != enable)
				SetHashDirty();

			m_config.stencilEnable = enable;
			return true;
		}

		bool DepthStencilState::EnableDepthStencilTest(bool enable)
		{
			if (!EnableDepthTest(enable))
				return false;
			if (!EnableStenciltest(enable))
				return false;
			return true;
		}

		bool DepthStencilState::SetStencilRef(const unsigned char ref)
		{
			if(ref != m_config.stencilRef)
				SetHashDirty();

			m_config.stencilRef = ref;
			return true;
		}

		size_t DepthStencilState::CalculateHashInternal()
		{
			size_t seed = 0;
			boost::hash_combine(seed, m_config.depthCmpFunc);
			boost::hash_combine(seed, m_config.depthTestEnable);
			boost::hash_combine(seed, m_config.depthWriteEnable);
			boost::hash_combine(seed, m_config.stencilEnable);
			boost::hash_combine(seed, m_config.stencilReadMask);
			boost::hash_combine(seed, m_config.stencilRef);
			boost::hash_combine(seed, m_config.stencilWriteMask);
			boost::hash_combine(seed, m_config.backFace.cmpFunc);
			boost::hash_combine(seed, m_config.backFace.depthFailOp);
			boost::hash_combine(seed, m_config.backFace.failOp);
			boost::hash_combine(seed, m_config.backFace.passOp);
			boost::hash_combine(seed, m_config.frontFace.cmpFunc);
			boost::hash_combine(seed, m_config.frontFace.depthFailOp);
			boost::hash_combine(seed, m_config.frontFace.failOp);
			boost::hash_combine(seed, m_config.frontFace.passOp);
			return seed;
		}
	}
}
