#pragma once
#include <memory>
#include "Container.h"
#include "Semantics.h"
#include "PipelineState.h"
#include "IMaterial.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		using SemanticSet = Container::UnorderedSet<RenderSemantics>;
		using ShaderParameterList = Container::Vector<ShaderParameter>;
		struct ShaderAndParameter
		{
			IShader* shader;
			ShaderParameterList parameters;
		};
		using MaterialShaderMap = Container::UnorderedMap<ShaderType, ShaderAndParameter>;
		class Material : public IMaterial
		{
		public:
			~Material()override;
			void SetShader(IShader* shader)override;
			void SetParameter(ShaderType type, const ShaderParameter& arg)override;
			void EnableBlend(bool enable)override;
			IShader* GetShader(ShaderType type);
			void GetShaders(Container::Vector<IShader*>& shaders);
			void RemoveShader(ShaderType type);
			const MaterialShaderMap& GetMaterialShaderMap()const;
			const BlendState& GetBlendState()const { return mBlendState; }
		protected:
			MaterialShaderMap mShaders;
			BlendState mBlendState;
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}
