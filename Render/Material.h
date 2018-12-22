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
			~Material()override;
			void SetShader(IShader* shader)override;
			bool ResetShader(ShaderType type)override;
			IShader* GetShader(ShaderType type)override;
			bool SetParameter(ShaderType type, const IShaderParameter* parameter)override;
			void EnableBlend(bool enable)override;
			void GetBlendState(BlendState& state)const override{ state = mBlendState; }
			std::size_t GetParameterCount(ShaderType type)const override;
			IShaderParameter* GetParameter(ShaderType type, std::size_t parameterIndex)override;
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
