#pragma once
#include <memory>
#include "Semantics.h"
#include "IMaterial.h"
#include "RefObject.h"
#include "ShaderParameter.h"

namespace Lightning
{
	namespace Render
	{
		class Material : public IMaterial
		{
		public:
			Material();
			~Material()override;
			void SetShader(const std::shared_ptr<IShader>& shader)override;
			bool ResetShader(ShaderType type)override;
			std::shared_ptr<IShader> GetShader(ShaderType type)override;
			bool SetParameter(ShaderType type, const IShaderParameter* parameter)override;
			void EnableBlend(bool enable)override;
			void GetBlendState(BlendState& state)const override{ state = mBlendState; }
			std::size_t GetParameterCount(ShaderType type)const override;
			const IShaderParameter* GetParameter(ShaderType type, std::size_t parameterIndex)const override;
		protected:
			struct ShaderParameters
			{
				std::shared_ptr<IShader> shader;
				std::vector<ShaderParameter> parameters;
			};
			using ShaderParametersCache = std::unordered_map<ShaderType, ShaderParameters>;

			ShaderParametersCache mShaders;
			BlendState mBlendState;
		};
	}
}
