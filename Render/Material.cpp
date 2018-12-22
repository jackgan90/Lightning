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
			for (auto& pair : mShaders)
			{
				pair.second.shader->Release();
			}
		}

		IShader* Material::GetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it == mShaders.end())
				return nullptr;
			return it->second.shader;
		}

		void Material::SetShader(IShader* shader)
		{
			if (!shader)
				return;
			shader->AddRef();
			ResetShader(shader->GetType());
			mShaders[shader->GetType()].shader = shader;
		}

		bool Material::ResetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				it->second.shader->Release();
				mShaders.erase(it);
				return true;
			}
			return false;
		}

		bool Material::SetParameter(ShaderType type, const ShaderParameter& arg)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				it->second.parameters.push_back(arg);
				return true;
			}
			return false;
		}

		bool Material::SetParameter(ShaderType type, const char* name, float value)
		{
			return SetParameter(type, ShaderParameter(name, value));
		}

		bool Material::SetParameter(ShaderType type, const char* name, const Vector2f& value)
		{
			return SetParameter(type, ShaderParameter(name, value));
		}

		bool Material::SetParameter(ShaderType type, const char* name, const Vector3f& value)
		{
			return SetParameter(type, ShaderParameter(name, value));
		}

		bool Material::SetParameter(ShaderType type, const char* name, const Vector4f& value)
		{
			return SetParameter(type, ShaderParameter(name, value));
		}

		bool Material::SetParameter(ShaderType type, const char* name, const Matrix4f& value)
		{
			return SetParameter(type, ShaderParameter(name, value));
		}

		bool Material::SetParameter(ShaderType type, const char* name, ITexture* texture)
		{
			return SetParameter(type, ShaderParameter(name, texture));
		}

		bool Material::SetParameter(ShaderType type, const char* name, const SamplerState& samplerState)
		{
			return SetParameter(type, ShaderParameter(name, samplerState));
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