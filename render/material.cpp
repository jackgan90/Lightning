#include "material.h"

namespace Lightning
{
	namespace Render
	{
		IShader* Material::GetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it == mShaders.end())
				return nullptr;
			return it->second.get();
		}

		void Material::SetShader(const SharedShaderPtr& pShader)
		{
			if (!pShader)
				return;
			mShaders[pShader->GetType()] = pShader;
		}

		void Material::RemoveShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
				mShaders.erase(it);
		}


	}
}