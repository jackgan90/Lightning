#pragma once
#include <string>
#include "rendererexportdef.h"
#include "irenderresourcekeeper.h"
#include "shaderdefine.h"
#include "filesystem.h"

namespace LightningGE
{
	namespace Renderer
	{
		enum ShaderType
		{
			SHADER_TYPE_VERTEX,
			SHADER_TYPE_FRAGMENT,
			SHADER_TYPE_GEOMETRY,
			SHADER_TYPE_HULL, //tesselation control shader in OpenGL
			SHADER_TYPE_DOMAIN	//tesselation evaluation shader in OpenGL
		};
		class LIGHTNINGGE_RENDERER_API IShader : public IRenderResourceKeeper
		{
		public:
			virtual std::string GetEntryPoint()const = 0; 
			virtual void SetEntryPoint(const std::string& entryPoint) = 0;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacro(const ShaderDefine& define) = 0;
			virtual const ShaderDefine GetMacro()const = 0;
			virtual bool Compile(const Foundation::FilePtr& file, const ShaderDefine& define) = 0;
			virtual const std::string GetCompileErrorLog()const = 0;
			virtual std::string GetName()const = 0;
#ifdef DEBUG
			virtual std::string GetSource()const = 0;
#endif
			virtual void GetShaderModelVersion(int& major, int& minor) = 0;
		};

		using ShaderPtr = std::shared_ptr<IShader>;
	}
}