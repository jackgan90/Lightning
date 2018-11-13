#pragma once
#include <exception>
#include "rendererexportdef.h"
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		class LIGHTNING_RENDER_API RendererException : public std::exception
		{
		public:
			explicit RendererException(const char*const w) : std::exception(w)
			{
				LOG_ERROR(w);
			}
		};
	}
}
