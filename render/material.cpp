#include "material.h"

namespace Lightning
{
	namespace Render
	{
		Material::Material()
		{

		}

		IShader* Material::GetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it == mShaders.end())
				return nullptr;
			return it->second.shader.get();
		}

		void Material::SetShader(const SharedShaderPtr& pShader)
		{
			if (!pShader)
				return;
			mShaders[pShader->GetType()].shader = pShader;
		}

		void Material::RemoveShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
				mShaders.erase(it);
		}

		void Material::SetArgument(ShaderType type, const ShaderArgument& arg)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				it->second.arguments.push_back(arg);
			}
		}

		const MaterialShaderMap& Material::GetMaterialShaderMap()const
		{
			return mShaders;
		}

		void Material::EnableBlend(bool enable)
		{
			mBlendState.enable = enable;
			mBlendState.alphaOp = BlendOperation::ADD;
			mBlendState.colorOp = BlendOperation::ADD;
			mBlendState.srcColorFactor = BlendFactor::SRC_ALPHA;
			mBlendState.srcAlphaFactor = BlendFactor::SRC_ALPHA;
			mBlendState.destColorFactor = BlendFactor::INV_SRC_ALPHA;
			mBlendState.destAlphaFactor = BlendFactor::INV_SRC_ALPHA;
		}
	}
}