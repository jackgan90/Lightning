#pragma once
#include <memory>
#include "Container.h"
#include "Semantics.h"
#include "IMaterial.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
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
				Container::Vector<ShaderParameter> parameters;
			};
			using ShaderParametersCache = Container::UnorderedMap<ShaderType, ShaderParameters>;

			ShaderParametersCache mShaders;
			BlendState mBlendState;
			REF_OBJECT_OVERRIDE(Material)
		};
	}
}
