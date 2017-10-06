#pragma once
#include "ishadermanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		//we should not only care about the shader name but also its' defined macros since every different macro group needs to compile the shader again
		typedef std::unordered_map<std::size_t, ShaderPtr> ShaderMap;
		class ShaderManager : public IShaderManager
		{
		public:
			void ReleaseRenderResources()override;
		protected:
			ShaderMap m_shaders;
		};
	}
}