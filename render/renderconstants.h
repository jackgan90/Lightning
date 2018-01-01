#pragma once
#include "ishader.h"

namespace LightningGE
{
	namespace Render
	{
		const int DEFAULT_SHADER_MODEL_MAJOR_VERSION = 5;
		const int DEFAULT_SHADER_MODEL_MINOR_VERSION = 0;
		const char* const DEFAULT_SHADER_ENTRY = "main";
		void GetShaderModelString(char* buf, ShaderType type, const int majorVersion, const int minorVersion);
	}
}