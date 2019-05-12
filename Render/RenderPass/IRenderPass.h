#pragma once
#include "IDrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		struct IRenderPass
		{
			virtual ~IRenderPass() = default;
			virtual void Apply(DrawCommandQueue& drawCommands) = 0;
		};
	}
}