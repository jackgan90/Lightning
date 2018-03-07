#include "material.h"

namespace LightningGE
{
	namespace Render
	{
		IShader* Material::GetShader(ShaderType type)
		{
			auto it = m_shaders.find(type);
			if (it == m_shaders.end())
				return nullptr;
			return it->second.get();
		}

		void Material::SetShader(const SharedShaderPtr& pShader)
		{
			if (!pShader)
				return;
			m_shaders[pShader->GetType()] = pShader;
		}

		void Material::RemoveShader(ShaderType type)
		{
			auto it = m_shaders.find(type);
			if (it != m_shaders.end())
				m_shaders.erase(it);
		}


	}
}