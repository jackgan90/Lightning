#pragma once
#include "IShader.h"
#include "VertexBuffer.h"

namespace Lightning
{
	namespace Render
	{
		unsigned int GetVertexFormatSize(RenderFormat format);
		void GetShaderModelString(char* buf, ShaderType type, const int majorVersion, const int minorVersion);
	}
}