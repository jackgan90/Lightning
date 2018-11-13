#pragma once
#include <string>
#include <exception>
#include <boost/functional/hash.hpp>
#include "rendererexportdef.h"
#include "shaderdefine.h"
#include "filesystem.h"
#include "HashableObject.h"
#include "semantics.h"
#include "renderexception.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector2f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		using Foundation::Math::Matrix3f;
		using Foundation::Math::Matrix4f;
		class LIGHTNING_RENDER_API ShaderCompileException : public RendererException
		{
		public:
			ShaderCompileException(const char*const w):RendererException(w){}
		};

		using Utility::HashableObject;
		enum class ShaderType
		{
			VERTEX = 1,
			FRAGMENT,
			GEOMETRY,
			TESSELATION_CONTROL, //hull shader
			TESSELATION_EVALUATION	//domain shader
		};

		enum class ShaderArgumentType
		{
			UNKNOWN,
			FLOAT,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			MATRIX2,
			MATRIX3,
			MATRIX4,
			TEXTURE2D,
			TEXTURE3D,
			SAMPLER2D,
			SAMPLER3D
		};
		using ShaderArgumentRegister = std::uint8_t;
		using ShaderArgumentSpace = std::uint8_t;

		struct LIGHTNING_RENDER_API ShaderArgument
		{
			ShaderArgumentType type;
			ShaderArgumentRegister registerIndex;
			ShaderArgumentSpace registerSpace;
			std::string name;
			union
			{
				float f;
				Vector2f v2;
				Vector3f v3;
				Vector4f v4;
				Matrix3f m3;
				Matrix4f m4;
				//TODO : add texture and sampler
			};
			ShaderArgument():type(ShaderArgumentType::UNKNOWN){}
			ShaderArgument(const std::string& n, const float _f):name(n), type(ShaderArgumentType::FLOAT), f(_f){}
			ShaderArgument(const std::string& n, const Vector2f& _v2) :name(n), type(ShaderArgumentType::FLOAT2) { new (&v2)Vector2f(_v2); }
			ShaderArgument(const std::string& n, const Vector3f& _v3) :name(n), type(ShaderArgumentType::FLOAT3) { new (&v3)Vector3f(_v3); }
			ShaderArgument(const std::string& n, const Vector4f& _v4) :name(n), type(ShaderArgumentType::FLOAT4) { new (&v4)Vector4f(_v4); }
			ShaderArgument(const std::string& n, const Matrix3f& _m3) :name(n), type(ShaderArgumentType::MATRIX3) { new (&m3)Matrix3f(_m3); }
			ShaderArgument(const std::string& n, const Matrix4f& _m4) :name(n), type(ShaderArgumentType::MATRIX4) { new (&m4)Matrix4f(_m4); }
			const std::uint8_t* Buffer(std::size_t& size)const
			{
				size = 0;
				switch (type)
				{
				case ShaderArgumentType::UNKNOWN:
					break;
				case ShaderArgumentType::FLOAT:
					size = sizeof(f);
					return reinterpret_cast<const std::uint8_t*>(&f);
				case ShaderArgumentType::FLOAT2:
					size = sizeof(v2);
					return reinterpret_cast<const std::uint8_t*>(&v2);
				case ShaderArgumentType::FLOAT3:
					size = sizeof(v3);
					return reinterpret_cast<const std::uint8_t*>(&v3);
				case ShaderArgumentType::FLOAT4:
					size = sizeof(v4);
					return reinterpret_cast<const std::uint8_t*>(&v4);
				case ShaderArgumentType::MATRIX2:
					break;
				case ShaderArgumentType::MATRIX3:
					size = sizeof(m3);
					return reinterpret_cast<const std::uint8_t*>(&m3);
				case ShaderArgumentType::MATRIX4:
					size = sizeof(m4);
					return reinterpret_cast<const std::uint8_t*>(&m4);
				case ShaderArgumentType::TEXTURE2D:
					break;
				case ShaderArgumentType::TEXTURE3D:
					break;
				case ShaderArgumentType::SAMPLER2D:
					break;
				case ShaderArgumentType::SAMPLER3D:
					break;
				default:
					break;
				}
				return nullptr;
			}
		};

		class LIGHTNING_RENDER_API IShader : public HashableObject
		{
		public:
			virtual std::string GetEntryPoint()const = 0; 
			virtual void SetEntryPoint(const std::string& entryPoint) = 0;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacros(const ShaderDefine& define) = 0;
			virtual const ShaderDefine GetMacros()const = 0;
			virtual std::size_t GetArgumentCount()const = 0;
			virtual void Compile() = 0;
			virtual std::string GetName()const = 0;
			virtual bool SetArgument(const ShaderArgument& argument) = 0;
			virtual const char* const GetSource()const = 0;
			virtual void GetShaderModelVersion(int& major, int& minor) = 0;
		};

		using SharedShaderPtr = std::shared_ptr<IShader>;

		class LIGHTNING_RENDER_API Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap);
			Shader(ShaderType type, const std::string& name, const std::string& entryPoint, const char* const source);
			~Shader()override;
			void SetEntryPoint(const std::string& entryPoint)override;
			void DefineMacros(const ShaderDefine& define)override;
			std::string GetEntryPoint()const override { return mEntryPoint; }
			ShaderType GetType()const override;
			const char* const GetSource()const override;
			std::string GetName()const override;
			void GetShaderModelVersion(int& major, int& minor)override;
		protected:
			size_t CalculateHashInternal()override;
			ShaderType mType;
			std::string mName;
			std::string mEntryPoint;
			const char* const mSource;
			ShaderDefine mMacros;
			int mShaderModelMajorVersion;
			int mShaderModelMinorVersion;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::IShader>
	{
		std::size_t operator()(const Lightning::Render::IShader& shader)const noexcept
		{
			return Lightning::Render::Shader::Hash(shader.GetType(), shader.GetName(), shader.GetMacros());
		}
	};
}
