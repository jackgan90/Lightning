#pragma once
#include <exception>
#include "rendererexportdef.h"
#include "logger.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::logger;
		using Foundation::LogLevel;
		class LIGHTNING_RENDER_API RendererException : public std::exception
		{
		public:
			explicit RendererException(const char*const w) : std::exception(w)
			{
				logger.Log(LogLevel::Error, w);
			}
		};
	}
}
