#pragma once
#include "PipelineState.h"
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		struct IMaterial : Plugins::IRefObject
		{
			//set shader used by this material
			virtual void INTERFACECALL SetShader(IShader* shader) = 0;
			//reset shader of specific type used by this material(reverse operation of SetShader)
			virtual bool INTERFACECALL ResetShader(ShaderType) = 0;
			virtual IShader* INTERFACECALL GetShader(ShaderType) = 0;
			virtual bool INTERFACECALL SetParameter(ShaderType type, const IShaderParameter* parameter) = 0;
			virtual void INTERFACECALL EnableBlend(bool enable) = 0;
			virtual void INTERFACECALL GetBlendState(BlendState&)const = 0;
			virtual std::size_t INTERFACECALL GetParameterCount(ShaderType type)const = 0;
			virtual const IShaderParameter* INTERFACECALL GetParameter(ShaderType type, std::size_t parameterIndex)const = 0;
		};
	}
}