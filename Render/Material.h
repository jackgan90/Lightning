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
		using ShaderArgumentList = Container::Vector<ShaderArgument>;
		struct ShaderAndArgument
		{
			IShader* shader;
			ShaderArgumentList arguments;
		};
		using MaterialShaderMap = Container::UnorderedMap<ShaderType, ShaderAndArgument>;
		class Material : public IMaterial
		{
		public:
			~Material()override;
			void RequireSemantic(RenderSemantics semantic) override{ mSemantics.emplace(semantic); }
			void SetShader(IShader* shader)override;
			void SetArgument(ShaderType type, const ShaderArgument& arg)override;
			void EnableBlend(bool enable)override;
			const SemanticSet& GetSemanticRequirements()const { return mSemantics; }
			IShader* GetShader(ShaderType type);
			void GetShaders(Container::Vector<IShader*>& shaders);
			void RemoveShader(ShaderType type);
			const MaterialShaderMap& GetMaterialShaderMap()const;
			const BlendState& GetBlendState()const { return mBlendState; }
		protected:
			SemanticSet mSemantics;
			MaterialShaderMap mShaders;
			BlendState mBlendState;
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}
