#pragma once
#include <memory>
#include "container.h"
#include "rendererexportdef.h"
#include "semantics.h"
#include "ishader.h"
#include "pipelinestate.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		using SemanticSet = container::unordered_set<RenderSemantics>;
		using ShaderArgumentList = container::vector<ShaderArgument>;
		struct ShaderAndArgument
		{
			SharedShaderPtr shader;
			ShaderArgumentList arguments;
		};
		using MaterialShaderMap = container::unordered_map<ShaderType, ShaderAndArgument>;
		class LIGHTNING_RENDER_API Material
		{
		public:
			Material();
			void RequireSemantic(RenderSemantics semantic) { mSemantics.emplace(semantic); }
			const SemanticSet& GetSemanticRequirements()const { return mSemantics; }
			IShader* GetShader(ShaderType type);
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
