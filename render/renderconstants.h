#pragma once
#include "ishader.h"

namespace LightningGE
{
	namespace Render
	{
		constexpr const int DEFAULT_SHADER_MODEL_MAJOR_VERSION = 5;
		constexpr const int DEFAULT_SHADER_MODEL_MINOR_VERSION = 0;
		constexpr const int MAX_GEOMETRY_BUFFER_COUNT = 8;
		const char* const DEFAULT_SHADER_ENTRY = "main";
		void GetShaderModelString(char* buf, ShaderType type, const int majorVersion, const int minorVersion);
	}
}