#pragma once
#include "RefCount.h"
#include "IShader.h"

namespace Lightning
{
	namespace Render
	{
		using Plugins::RefCount;
		class IMaterial : public RefCount
		{
		public:
			virtual void RequireSemantic(RenderSemantics semantic) = 0;
			virtual void SetShader(IShader* shader) = 0;
			virtual void SetArgument(ShaderType type, const ShaderArgument& arg) = 0;
			virtual void EnableBlend(bool enable) = 0;
		};
	}
}