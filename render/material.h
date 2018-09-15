#pragma once
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "rendererexportdef.h"
#include "semantics.h"
#include "ishader.h"

namespace Lightning
{
	namespace Render
	{
		using SemanticSet = std::unordered_set<RenderSemantics>;
		class LIGHTNING_RENDER_API Material
		{
		public:
			void RequireSemantic(RenderSemantics semantic) { m_semantics.emplace(semantic); }
			const SemanticSet& GetSemanticRequirements()const { return m_semantics; }
			IShader* GetShader(ShaderType type);
			void SetShader(const SharedShaderPtr& pShader);
			void RemoveShader(ShaderType type);
		protected:
			using MaterialShaderMap = std::unordered_map<ShaderType, SharedShaderPtr>;
			SemanticSet m_semantics;
			MaterialShaderMap m_shaders;
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}
