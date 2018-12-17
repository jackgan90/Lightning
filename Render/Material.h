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
			~Material()override;
			void SetShader(IShader* shader)override;
			bool ResetShader(ShaderType type)override;
			IShader* GetShader(ShaderType type)override;
			void GetShaders(Container::Vector<IShader*>& shaders)override;
			bool SetParameter(ShaderType type, const ShaderParameter& arg)override;
			void EnableBlend(bool enable)override;
			void GetBlendState(BlendState& state)const override{ state = mBlendState; }
			const ShaderParametersCache& GetAllShaderParameters()const override { return mShaders; }
		protected:
			ShaderParametersCache mShaders;
			BlendState mBlendState;
		};
	}
}
