#include <sstream>
#include "rendererhelper.h"

namespace Lightning
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
#ifdef _MSC_VER
			strcpy_s(buf, ss.str().length() + 1, ss.str().c_str());
#else
			std::strcpy(buf, ss.str().c_str());
#endif
		}
	}
}