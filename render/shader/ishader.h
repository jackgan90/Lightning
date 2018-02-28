#pragma once
#include <string>
#include <exception>
#include "rendererexportdef.h"
#include "shaderdefine.h"
#include "filesystem.h"
#include "hashableobject.h"
#include "types/vector.h"

namespace LightningGE
{
	namespace Render
	{
		using Utility::HashableObject;
		class IShaderManager;
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

		struct LIGHTNINGGE_RENDER_API ShaderArgument
		{
			LIGHTNINGGE_ALIGNED_OPERATOR_NEW
			ShaderArgumentType type;
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
			virtual std::size_t GetInputArgumentCount()const = 0;
			//virtual bool Compile(const Foundation::SharedFilePtr& file, const ShaderDefine& define) = 0;
			//virtual const std::string GetCompileErrorLog()const = 0;
			virtual std::string GetName()const = 0;
#ifndef NDEBUG
			virtual const char* const GetSource()const = 0;
#endif
			virtual void GetShaderModelVersion(int& major, int& minor) = 0;
		};

		using SharedShaderPtr = std::shared_ptr<IShader>;

		class LIGHTNINGGE_RENDER_API Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap);
			Shader(const std::string& entryPoint);
			~Shader()override;
			void SetEntryPoint(const std::string& entryPoint)override;
			void DefineMacros(const ShaderDefine& define)override;
			std::string GetEntryPoint()const override { return m_entryPoint; }
		protected:
			size_t CalculateHashInternal()override;
			std::string m_entryPoint;
			ShaderDefine m_macros;
		};
	}
}