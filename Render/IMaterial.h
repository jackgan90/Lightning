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

		class IMaterial : public Plugins::RefCount
		{
		public:
			//set shader used by this material
			virtual void SetShader(IShader* shader) = 0;
			//reset shader of specific type used by this material(reverse operation of SetShader)
			virtual bool ResetShader(ShaderType) = 0;
			virtual IShader* GetShader(ShaderType) = 0;
			virtual void GetShaders(Container::Vector<IShader*>& shaders) = 0;
			virtual bool SetParameter(ShaderType type, const ShaderParameter& arg) = 0;
			virtual void EnableBlend(bool enable) = 0;
			virtual void GetBlendState(BlendState&)const = 0;
			virtual const ShaderParametersCache& GetAllShaderParameters()const = 0;
			template<typename... ParameterType>
			bool SetParameter(ShaderType type, const std::string& name, ParameterType&&... parameter)
			{
				return SetParameter(type, ShaderParameter(name, std::forward<ParameterType>(parameter)...));
			}
		};
	}
}