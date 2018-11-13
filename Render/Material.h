#pragma once
#include <memory>
#include "Container.h"
#include "RendererExportDef.h"
#include "Semantics.h"
#include "IShader.h"
#include "PipelineState.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		using SemanticSet = Container::UnorderedSet<RenderSemantics>;
		using ShaderArgumentList = Container::Vector<ShaderArgument>;
		struct ShaderAndArgument
		{
			SharedShaderPtr shader;
			ShaderArgumentList arguments;
		};
		using MaterialShaderMap = Container::UnorderedMap<ShaderType, ShaderAndArgument>;
		class LIGHTNING_RENDER_API Material
		{
		public:
			Material();
			void RequireSemantic(RenderSemantics semantic) { mSemantics.emplace(semantic); }
			const SemanticSet& GetSemanticRequirements()const { return mSemantics; }
			IShader* GetShader(ShaderType type);
			void GetShaders(Container::Vector<IShader*>& shaders);
			void SetShader(const SharedShaderPtr& pShader);
			void RemoveShader(ShaderType type);
			void SetArgument(ShaderType type, const ShaderArgument& arg);
			const MaterialShaderMap& GetMaterialShaderMap()const;
			void EnableBlend(bool enable);
			const BlendState& GetBlendState()const { return mBlendState; }
		protected:
			SemanticSet mSemantics;
			MaterialShaderMap mShaders;
			BlendState mBlendState;
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}
