#pragma once
#include <string>
#include <exception>
#include <boost/functional/hash.hpp>
#include "Semantics.h"
#include "RenderException.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "IRefObject.h"
#include "Texture/ITexture.h"
#include "Texture/Sampler.h"
#include "IShaderMacros.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector2f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		//using Foundation::Math::Matrix3f;
		using Foundation::Math::Matrix4f;
		class ShaderCompileException : public RendererException
		{
		public:
			ShaderCompileException(const char*const w):RendererException(w){}
		};

		enum class ShaderType
		{
			VERTEX = 1,
			FRAGMENT,
			GEOMETRY,
			TESSELATION_CONTROL, //hull shader
			TESSELATION_EVALUATION	//domain shader
		};

		enum class ShaderParameterType
		{
			UNKNOWN,
			FLOAT,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			//MATRIX2,
			//MATRIX3,
			MATRIX4,
			TEXTURE,
			SAMPLER,
		};

		struct IShaderParameter
		{
			virtual INTERFACECALL ~IShaderParameter() = default;
			virtual const char* INTERFACECALL GetName()const = 0;
			virtual ShaderParameterType INTERFACECALL GetType()const = 0;
			virtual const void* INTERFACECALL Buffer(std::size_t& size)const = 0;
		};

		class ShaderParameter : public IShaderParameter
		{
		public:
			ShaderParameter():mType(ShaderParameterType::UNKNOWN){}
			ShaderParameter(const char* n, const float _f):mName(n), mType(ShaderParameterType::FLOAT), f(_f){}
			ShaderParameter(const char* n, const Vector2f& _v2) :mName(n), mType(ShaderParameterType::FLOAT2) { new (&v2)Vector2f(_v2); }
			ShaderParameter(const char* n, const Vector3f& _v3) :mName(n), mType(ShaderParameterType::FLOAT3) { new (&v3)Vector3f(_v3); }
			ShaderParameter(const char* n, const Vector4f& _v4) :mName(n), mType(ShaderParameterType::FLOAT4) { new (&v4)Vector4f(_v4); }
			ShaderParameter(const char* n, const Matrix4f& _m4) :mName(n), mType(ShaderParameterType::MATRIX4) { new (&m4)Matrix4f(_m4); }
			ShaderParameter(const char* n, ITexture* _texture) :mName(n), mType(ShaderParameterType::TEXTURE) { texture = _texture; }
			ShaderParameter(const char* n, const SamplerState& _state) :mName(n), mType(ShaderParameterType::SAMPLER) { samplerState = _state; }
			ShaderParameter(const char* n, ShaderParameterType type, const void* buffer, std::size_t bufferSize) : mName(n), mType(type)
			{
				std::memcpy(&f, buffer, bufferSize);
			}
			const char* INTERFACECALL GetName()const override
			{
				return mName.c_str();
			}
			ShaderParameterType INTERFACECALL GetType()const override
			{
				return mType;
			}

			const void* INTERFACECALL Buffer(std::size_t& size)const override
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
					return texture;
				case ShaderParameterType::SAMPLER:
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

		struct IShader :  Plugins::IRefObject
		{
			virtual ShaderType INTERFACECALL GetType()const = 0;
			virtual void INTERFACECALL DefineMacros(const IShaderMacros* macros) = 0;
			virtual const IShaderMacros* INTERFACECALL GetMacros()const = 0;
			virtual std::size_t INTERFACECALL GetParameterCount()const = 0;
			virtual void INTERFACECALL Compile() = 0;
			virtual const char* INTERFACECALL GetName()const = 0;
			virtual bool INTERFACECALL SetParameter(const IShaderParameter* parameter) = 0;
			virtual const char* const INTERFACECALL GetSource()const = 0;
			virtual void INTERFACECALL GetShaderModelVersion(int& major, int& minor) = 0;
			virtual void INTERFACECALL GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount) = 0;
			virtual std::size_t INTERFACECALL GetHash()const = 0;
		};
	}
}

