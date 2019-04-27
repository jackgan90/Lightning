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
			INTERFACECALL ~Material()override;
			void INTERFACECALL SetShader(IShader* shader)override;
			bool INTERFACECALL ResetShader(ShaderType type)override;
			IShader* INTERFACECALL GetShader(ShaderType type)override;
			bool INTERFACECALL SetParameter(ShaderType type, const IShaderParameter* parameter)override;
			void INTERFACECALL EnableBlend(bool enable)override;
			void INTERFACECALL GetBlendState(BlendState& state)const override{ state = mBlendState; }
			std::size_t INTERFACECALL GetParameterCount(ShaderType type)const override;
			const IShaderParameter* INTERFACECALL GetParameter(ShaderType type, std::size_t parameterIndex)const override;
		protected:
			struct ShaderParameters
			{
				IShader* shader;
				std::vector<ShaderParameter> parameters;
			};
			using ShaderParametersCache = std::unordered_map<ShaderType, ShaderParameters>;

			ShaderParametersCache mShaders;
			BlendState mBlendState;
			REF_OBJECT_OVERRIDE(Material)
		};
	}
}
