#pragma once
#include <string>
#include "rendererexportdef.h"
#include "shaderdefine.h"
#include "filesystem.h"
#include "hashableobject.h"

namespace LightningGE
{
	namespace Renderer
	{
		using Utility::HashableObject;
		enum ShaderType
		{
			SHADER_TYPE_VERTEX,
			SHADER_TYPE_FRAGMENT,
			SHADER_TYPE_GEOMETRY,
			SHADER_TYPE_HULL, //tesselation control shader in OpenGL
			SHADER_TYPE_DOMAIN	//tesselation evaluation shader in OpenGL
		};
		class LIGHTNINGGE_RENDERER_API IShader : public HashableObject
		{
		public:
			virtual std::string GetEntryPoint()const = 0; 
			virtual void SetEntryPoint(const std::string& entryPoint) = 0;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacros(const ShaderDefine& define) = 0;
			virtual const ShaderDefine GetMacros()const = 0;
			virtual bool Compile(const Foundation::FilePtr& file, const ShaderDefine& define) = 0;
			virtual const std::string GetCompileErrorLog()const = 0;
			virtual std::string GetName()const = 0;
#ifndef NDEBUG
			virtual std::string GetSource()const = 0;
#endif
			virtual void GetShaderModelVersion(int& major, int& minor) = 0;
		};

		typedef std::shared_ptr<IShader> ShaderPtr;

		class LIGHTNINGGE_RENDERER_API Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const ShaderDefine& defineMap);
			Shader();
			void SetEntryPoint(const std::string& entryPoint)override;
			void DefineMacros(const ShaderDefine& define)override;
		protected:
			size_t CalculateHashInternal()override;
			std::string m_entryPoint;
			ShaderDefine m_macros;
		};
	}
}