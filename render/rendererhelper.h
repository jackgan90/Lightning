#pragma once
#include "ishader.h"
#include "vertexbuffer.h"

namespace Lightning
{
	namespace Render
	{
		unsigned int GetVertexFormatSize(RenderFormat format);
		void GetShaderModelString(char* buf, ShaderType type, const int majorVersion, const int minorVersion);
	}
}