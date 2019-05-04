#pragma once
#include "IShaderParameter.h"
#include "Vector.h"
#include "Matrix.h"
#include "Texture/ITexture.h"
#include "Texture/Sampler.h"

namespace Lightning
{
	namespace Render
	{
		using namespace Foundation::Math;
		class ShaderParameter : public IShaderParameter
		{
		public:
			ShaderParameter():mType(ShaderParameterType::UNKNOWN){}
			ShaderParameter(const std::string& name, const float _f):mName(name), mType(ShaderParameterType::FLOAT), f(_f){}
			ShaderParameter(const std::string& name, const Vector2f& _v2) :mName(name), mType(ShaderParameterType::FLOAT2) { new (&v2)Vector2f(_v2); }
			ShaderParameter(const std::string& name, const Vector3f& _v3) :mName(name), mType(ShaderParameterType::FLOAT3) { new (&v3)Vector3f(_v3); }
			ShaderParameter(const std::string& name, const Vector4f& _v4) :mName(name), mType(ShaderParameterType::FLOAT4) { new (&v4)Vector4f(_v4); }
			ShaderParameter(const std::string& name, const Matrix4f& _m4) :mName(name), mType(ShaderParameterType::MATRIX4) { new (&m4)Matrix4f(_m4); }
			ShaderParameter(const std::string& name, ITexture* _texture) :mName(name), mType(ShaderParameterType::TEXTURE) { texture = _texture; }
			ShaderParameter(const std::string& name, const SamplerState& _state) :mName(name), mType(ShaderParameterType::SAMPLER) { samplerState = _state; }
			ShaderParameter(const std::string& name, ShaderParameterType type, const void* buffer, std::size_t bufferSize) : mName(name), mType(type)
			{
				if (type == ShaderParameterType::TEXTURE)
				{
					assert(bufferSize == sizeof(texture) && "BufferSize mismatched.");
					texture = const_cast<ITexture*>(reinterpret_cast<const ITexture*>(buffer));
				}
				else
				{
					std::memcpy(&f, buffer, bufferSize);
				}
			}
			std::string GetName()const override
			{
				return mName;
			}
			ShaderParameterType GetType()const override
			{
				return mType;
			}

			const void* Buffer(std::size_t& size)const override
			{
				size = 0;
				switch (mType)
				{
				case ShaderParameterType::UNKNOWN:
					break;
				case ShaderParameterType::FLOAT:
					size = sizeof(f);
					return &f;
				case ShaderParameterType::FLOAT2:
					size = sizeof(v2);
					return &v2;
				case ShaderParameterType::FLOAT3:
					size = sizeof(v3);
					return &v3;
				case ShaderParameterType::FLOAT4:
					size = sizeof(v4);
					return &v4;
				case ShaderParameterType::MATRIX4:
					size = sizeof(m4);
					return &m4;
				case ShaderParameterType::TEXTURE:
					size = sizeof(texture);
					return texture;
				case ShaderParameterType::SAMPLER:
					size = sizeof(samplerState);
					return &samplerState;
				default:
					break;
				}
				return nullptr;
			}
		private:
			ShaderParameterType mType;
			std::string mName;
			union
			{
				float f;
				Vector2f v2;
				Vector3f v3;
				Vector4f v4;
				Matrix4f m4;
				ITexture* texture;
				SamplerState samplerState;
			};
		};
	}
}
