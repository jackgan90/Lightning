#pragma once
#include <memory>
#include "Semantics.h"
#include "IMaterial.h"
#include "Parameter.h"

namespace Lightning
{
	namespace Render
	{
		class Material : public IMaterial
		{
		public:
			Material();
			~Material()override;
			void SetShader(ShaderType shaderType, const std::shared_ptr<IShader>& shader)override;
			std::shared_ptr<IShader> GetShader(ShaderType type)override;
			bool SetParameter(const Parameter& parameter)override;
			std::size_t GetParameterTypeCount(ParameterType parameterType)const override;
			bool GetParameter(const std::string& name, Parameter& parameter)const override;
			void EnableBlend(bool enable)override;
			void GetBlendState(BlendState& state)const override{ state = mBlendState; }
			void VisitParameters(std::function<void(const Parameter& parameter)> visitor)const override;
		protected:
			std::unordered_map<ShaderType, std::shared_ptr<IShader>> mShaders;
			std::unordered_map<std::string, Parameter> mParameters;
			BlendState mBlendState;
		};
	}
}
