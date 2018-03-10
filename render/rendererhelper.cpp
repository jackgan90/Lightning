#include "rendererhelper.h"

namespace LightningGE
{
	namespace Render
	{
		unsigned int GetVertexFormatSize(RenderFormat format)
		{
			switch (format)
			{
			case RenderFormat::R32G32B32_FLOAT:
				return 12;
			case RenderFormat::R32G32B32A32_FLOAT:
				return 16;
			default:
				return 0;
			}
		}

		void GetShaderModelString(char* buf, ShaderType type, const int majorVersion, const int minorVersion)
		{
			std::stringstream ss;
			switch (type)
			{
			case ShaderType::VERTEX:
				ss << "vs";
				break;
			case ShaderType::FRAGMENT:
				ss << "ps";
				break;
			case ShaderType::GEOMETRY:
				ss << "gs";
				break;
			case ShaderType::TESSELATION_CONTROL:
				ss << "hs";
				break;
			case ShaderType::TESSELATION_EVALUATION:
				ss << "ds";
				break;
			}
			ss << "_" << majorVersion << "_" << minorVersion;
			std::strcpy(buf, ss.str().c_str());
		}
	}
}