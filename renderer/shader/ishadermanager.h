#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "ishader.h"

namespace LightningGE
{
	namespace Renderer
	{
		class IShaderManager : public IRenderResourceKeeper
		{
		public:
			//return a compiled shader ptr if the specified shader can be created successfully
			virtual ShaderPtr GetShader(ShaderType type, const std::string& shaderName, const ShaderDefine& defineMap) = 0;
		protected:
			IShaderManager(){}
		};
		typedef std::shared_ptr<IShaderManager> ShaderManagerPtr;
	}
}