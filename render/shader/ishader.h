#pragma once
#include <string>
#include <exception>
#include "rendererexportdef.h"
#include "shaderdefine.h"
#include "filesystem.h"
#include "hashableobject.h"
#include "semantics.h"
#include "renderexception.h"
#include "memory/stackallocator.h"
#include "types/vector.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::StackAllocator;
		class LIGHTNINGGE_RENDER_API ShaderCompileException : public RendererException
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

		struct LIGHTNINGGE_RENDER_API ShaderArgument
		{
			LIGHTNINGGE_ALIGNED_OPERATOR_NEW
			ShaderArgumentType type;
			ShaderArgumentRegister registerIndex;
			ShaderArgumentSpace registerSpace;
			const char* name;
			union
			{
				float f;
				Vector2f v2;
				Vector3f v3;
				Vector4f v4;
				Matrix2x2f m2;
				Matrix3x3f m3;
				Matrix4x4f m4;
				//TODO : add texture and sampler
			};
			ShaderArgument(const float _f):type(ShaderArgumentType::FLOAT), f(_f){}
			ShaderArgument(const Vector2f& _v2) :type(ShaderArgumentType::FLOAT2) { new (&v2)Vector2f(_v2); }
			ShaderArgument(Vector2f&& _v2) :type(ShaderArgumentType::FLOAT2) { new (&v2)Vector2f(std::move(_v2)); }
			ShaderArgument(const Vector3f& _v3) :type(ShaderArgumentType::FLOAT3) { new (&v3)Vector3f(_v3); }
			ShaderArgument(Vector3f&& _v3) :type(ShaderArgumentType::FLOAT3) { new (&v3)Vector3f(std::move(_v3)); }
			ShaderArgument(const Vector4f& _v4) :type(ShaderArgumentType::FLOAT4) { new (&v4)Vector4f(_v4); }
			ShaderArgument(Vector4f&& _v4) :type(ShaderArgumentType::FLOAT4) { new (&v4)Vector4f(std::move(_v4)); }
			ShaderArgument(const Matrix2x2f& _m2) :type(ShaderArgumentType::MATRIX2) { new (&m2)Matrix2x2f(_m2); }
			ShaderArgument(Matrix2x2f&& _m2) :type(ShaderArgumentType::MATRIX2) { new (&m2)Matrix2x2f(std::move(_m2)); }
			ShaderArgument(const Matrix3x3f& _m3) :type(ShaderArgumentType::MATRIX3) { new (&m3)Matrix3x3f(_m3); }
			ShaderArgument(Matrix3x3f&& _m3) :type(ShaderArgumentType::MATRIX3) { new (&m3)Matrix3x3f(std::move(_m3)); }
			ShaderArgument(const Matrix4x4f& _m4) :type(ShaderArgumentType::MATRIX4) { new (&m4)Matrix4x4f(_m4); }
			ShaderArgument(Matrix4x4f&& _m4) :type(ShaderArgumentType::MATRIX4) { new (&m4)Matrix4x4f(std::move(_m4)); }
			float GetFloat()const { return f; }
			Vector2f GetVector2()const { return v2; }
			Vector3f GetVector3()const { return v3; }
			Vector4f GetVector4()const { return v4; }
			Matrix2x2f GetMatrix2()const { return m2; }
			Matrix3x3f GetMatrix3()const { return m3; }
			Matrix4x4f GetMatrix4()const { return m4; }
		};

		class LIGHTNINGGE_RENDER_API IShader : public HashableObject
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
			virtual void SetArgument(const ShaderArgument& argument) = 0;
			virtual const char* const GetSource()const = 0;
			virtual void GetShaderModelVersion(int& major, int& minor) = 0;
		};

		using SharedShaderPtr = std::shared_ptr<IShader>;

		class LIGHTNINGGE_RENDER_API Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap);
			Shader(ShaderType type, const std::string& name, const std::string& entryPoint, const char* const source);
			~Shader()override;
			void SetEntryPoint(const std::string& entryPoint)override;
			void DefineMacros(const ShaderDefine& define)override;
			std::string GetEntryPoint()const override { return m_entryPoint; }
			ShaderType GetType()const override;
			const char* const GetSource()const override;
			std::string GetName()const override;
			void GetShaderModelVersion(int& major, int& minor)override;
		protected:
			size_t CalculateHashInternal()override;
			ShaderType m_type;
			std::string m_name;
			std::string m_entryPoint;
			const char* const m_source;
			ShaderDefine m_macros;
			int m_smMajorVersion;
			int m_smMinorVersion;
			static StackAllocator<true, 16, 8192 > s_compileAllocator;
		};
	}
}