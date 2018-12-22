#pragma once
#include "PipelineState.h"
#include "IShader.h"
#include "Container.h"

namespace Lightning
{
	namespace Render
	{
		struct ShaderParameters
		{
			IShader* shader;
			Container::Vector<ShaderParameter> parameters;
		};

		using ShaderParametersCache = Container::UnorderedMap<ShaderType, ShaderParameters>;

		class IMaterial : public Plugins::IRefObject
		{
		public:
			//set shader used by this material
			virtual void SetShader(IShader* shader) = 0;
			//reset shader of specific type used by this material(reverse operation of SetShader)
			virtual bool ResetShader(ShaderType) = 0;
			virtual IShader* GetShader(ShaderType) = 0;
			virtual bool SetParameter(ShaderType type, const IShaderParameter* parameter) = 0;
			virtual void EnableBlend(bool enable) = 0;
			virtual void GetBlendState(BlendState&)const = 0;
			virtual const ShaderParametersCache& GetAllShaderParameters()const = 0;
		};
	}
}