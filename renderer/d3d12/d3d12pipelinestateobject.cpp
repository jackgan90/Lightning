#include "common.h"
#include "d3d12pipelinestateobject.h"
#include "d3d12blendstate.h"
#include "d3d12depthstencilstate.h"
#include "d3d12rasterizerstate.h"
#include "d3d12shader.h"

#define SYNCHRONIZE_SHADER(shader, shaderType, structMember) \
	auto shader = GetShader(shaderType); \
	if (shader) \
	{ \
		m_desc.##structMember.pShaderBytecode = STATIC_CAST_PTR(D3D12Shader, shader)->GetByteCodeBuffer(); \
		m_desc.##structMember.BytecodeLength = STATIC_CAST_PTR(D3D12Shader, shader)->GetByteCodeBufferSize(); \
	} \


namespace LightningGE
{
	namespace Renderer
	{

		void D3D12PipelineStateObject::SynchronizeGraphicsDesc()
		{
			m_desc.BlendState = STATIC_CAST_PTR(D3D12BlendState, m_blendState)->m_desc;
			m_desc.DepthStencilState = STATIC_CAST_PTR(D3D12DepthStencilState, m_depthStencilState)->m_desc;
			m_desc.RasterizerState = STATIC_CAST_PTR(D3D12RasterizerState, m_rasterizerState)->m_desc;
			SYNCHRONIZE_SHADER(vs, SHADER_TYPE_VERTEX, VS)
			SYNCHRONIZE_SHADER(ps, SHADER_TYPE_FRAGMENT, PS)
			SYNCHRONIZE_SHADER(gs, SHADER_TYPE_GEOMETRY, GS)
			SYNCHRONIZE_SHADER(hs, SHADER_TYPE_HULL, HS)
			SYNCHRONIZE_SHADER(ds, SHADER_TYPE_DOMAIN, DS)
		}

	}
}