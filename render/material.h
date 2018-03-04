#pragma once
#include <memory>
#include <unordered_set>
#include "rendererexportdef.h"
#include "semantics.h"

namespace LightningGE
{
	namespace Render
	{
		using SemanticSet = std::unordered_set<RenderSemantics>;
		class LIGHTNINGGE_RENDER_API Material
		{
		public:
			void RequireSemantic(RenderSemantics semantic) { m_semantics.emplace(semantic); }
			SemanticSet GetSemanticRequirements()const { return m_semantics; }
		protected:
			SemanticSet m_semantics;
		};
		using SharedMaterialPtr = std::shared_ptr<Material>;
	}
}