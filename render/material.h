#pragma once
#include <memory>
#include "container.h"
#include "rendererexportdef.h"
#include "semantics.h"
#include "ishader.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::container;
		using SemanticSet = container::unordered_set<RenderSemantics>;
		class LIGHTNING_RENDER_API Material
		{
		public:
			void RequireSemantic(RenderSemantics semantic) { mSemantics.emplace(semantic); }
			const SemanticSet& GetSemanticRequirements()const { return mSemantics; }
			IShader* GetShader(ShaderType type);
			void SetShader(const SharedShaderPtr& pShader);
			void RemoveShader(ShaderType type);
		protected:
			using MaterialShaderMap = container::unordered_map<ShaderType, SharedShaderPtr>;
			SemanticSet mSemantics;
			MaterialShaderMap mShaders;
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}
