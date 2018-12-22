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
			const ShaderParametersCache& GetAllShaderParameters()const override { return mShaders; }
		protected:
			ShaderParametersCache mShaders;
			BlendState mBlendState;
			REF_OBJECT_OVERRIDE(Material)
		};
	}
}
