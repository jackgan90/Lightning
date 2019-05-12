#pragma once
#include "IDrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		struct IRenderPass
		{
			virtual ~IRenderPass() = default;
			virtual void Render(DrawCommandQueue& drawCommands) = 0;
			virtual bool NeedRender()const = 0;
		};
	}
}