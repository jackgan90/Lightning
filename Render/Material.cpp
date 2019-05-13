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

		std::size_t Material::GetParameterTypeCount(ParameterType parameterType)const
		{
			std::size_t count{ 0 };
			std::for_each(mParameters.cbegin(), mParameters.cend(), 
				[&count](const std::pair<std::string, Parameter>&) {
				count += 1;
			});
			return count;
		}

		bool Material::GetParameter(const std::string& name, Parameter& parameter)const
		{
			auto it = mParameters.find(name);
			if (it != mParameters.end())
			{
				parameter = it->second;
				return true;
			}
			return false;
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