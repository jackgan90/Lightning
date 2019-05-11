#pragma once
#include "PipelineState.h"
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		struct IMaterial
		{
			virtual ~IMaterial() = default;
			//set shader used by this material
			virtual void SetShader(ShaderType shaderType, const std::shared_ptr<IShader>& shader) = 0;
			virtual std::shared_ptr<IShader> GetShader(ShaderType) = 0;
			virtual bool SetParameter(ShaderType type, const ShaderParameter& parameter) = 0;
			virtual void EnableBlend(bool enable) = 0;
			virtual void GetBlendState(BlendState&)const = 0;
			virtual std::size_t GetParameterCount(ShaderType type)const = 0;
			virtual const ShaderParameter* GetParameter(ShaderType type, std::size_t parameterIndex)const = 0;
		};
	}
}