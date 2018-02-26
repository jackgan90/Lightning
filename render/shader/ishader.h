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
		};

		class LIGHTNINGGE_RENDER_API IShader : public HashableObject
		{
		public:
			virtual std::string GetEntryPoint()const = 0; 
			virtual void SetEntryPoint(const std::string& entryPoint) = 0;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacros(const ShaderDefine& define) = 0;
			virtual const ShaderDefine GetMacros()const = 0;
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