#pragma once
#include <sstream>
#include "renderconstants.h"

namespace LightningGE
{
	namespace Renderer
	{
		void GetShaderModelString(char* buf, ShaderType type, const int majorVersion, const int minorVersion)
		{
			std::stringstream ss;
			switch (type)
			{
			case SHADER_TYPE_VERTEX:
				ss << "vs";
				break;
			case SHADER_TYPE_FRAGMENT:
				ss << "ps";
				break;
			case SHADER_TYPE_GEOMETRY:
				ss << "gs";
				break;
			case SHADER_TYPE_HULL:
				ss << "hs";
				break;
			case SHADER_TYPE_DOMAIN:
				ss << "ds";
				break;
			}
			ss << "_" << majorVersion << "_" << minorVersion;
			std::strcpy(buf, ss.str().c_str());
		}
	}
}