#include "Material.h"

namespace Lightning
{
	namespace Render
	{
		Material::Material()
		{

		}

		Material::~Material()
		{
		}

		std::shared_ptr<IShader> Material::GetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it == mShaders.end())
				return nullptr;
			return it->second;
		}

		void Material::SetShader(ShaderType shaderType, const std::shared_ptr<IShader>& shader)
		{
			if (!shader)
			{
				mShaders.erase(shaderType);
				return;
			}
			assert(shaderType == shader->GetType() && "Mismatched shader types.");
			mShaders[shader->GetType()] = shader;
		}

		bool Material::SetParameter(const Parameter& parameter)
		{
			mParameters[parameter.GetName()] = parameter;
			return true;
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

		void Material::VisitParameters(std::function<void(const Parameter& parameter)> visitor)const
		{
			if (!visitor)
				return;
			for (auto it = mParameters.cbegin(); it != mParameters.cend();++it)
			{
				visitor(it->second);
			}
		}
	}
}