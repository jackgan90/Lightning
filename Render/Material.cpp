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
			return it->second.shader;
		}

		void Material::SetShader(const std::shared_ptr<IShader>& shader)
		{
			if (!shader)
				return;
			ResetShader(shader->GetType());
			mShaders[shader->GetType()].shader = shader;
		}

		bool Material::ResetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				mShaders.erase(it);
				return true;
			}
			return false;
		}

		bool Material::SetParameter(ShaderType type, const IShaderParameter* parameter)
		{
			assert(parameter != nullptr && "shader parameter cannot be nullptr!");
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				auto parameterName = parameter->GetName();
				auto parameterType = parameter->GetType();
				std::size_t bufferSize;
				auto buffer = parameter->Buffer(bufferSize);
				assert(bufferSize > 0 && "Invalid parameter type.");
				it->second.parameters.push_back(ShaderParameter(parameterName, parameterType, buffer, bufferSize));
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

		std::size_t Material::GetParameterCount(ShaderType type)const
		{
			auto it = mShaders.find(type);
			if (it == mShaders.end())
				return 0;
			return it->second.parameters.size();
		}

		const IShaderParameter* Material::GetParameter(ShaderType type, std::size_t parameterIndex)const
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				return &it->second.parameters[parameterIndex];
			}
			return nullptr;
		}

	}
}