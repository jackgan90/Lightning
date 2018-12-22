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
			virtual void SetShader(IShader* shader) = 0;
			//reset shader of specific type used by this material(reverse operation of SetShader)
			virtual bool ResetShader(ShaderType) = 0;
			virtual IShader* GetShader(ShaderType) = 0;
			virtual bool SetParameter(ShaderType type, const IShaderParameter* parameter) = 0;
			virtual void EnableBlend(bool enable) = 0;
			virtual void GetBlendState(BlendState&)const = 0;
			virtual std::size_t GetParameterCount(ShaderType type)const = 0;
			virtual IShaderParameter* GetParameter(ShaderType type, std::size_t parameterIndex) = 0;
		};
	}
}