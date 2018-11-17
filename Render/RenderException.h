#pragma once
#include <exception>
#include "Logger.h"

namespace Lightning
{
	namespace Render
	{
		class RendererException : public std::exception
		{
		public:
			explicit RendererException(const char*const w) : std::exception(w)
			{
				LOG_ERROR(w);
			}
		};
	}
}
