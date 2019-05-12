#pragma once
#include <memory>
#include <vector>
#include "IRenderPass.h"

namespace Lightning
{
	namespace Render
	{
		class RenderPass : public IRenderPass
		{
		public:
			bool NeedRender()const;
		protected:
			std::vector<std::shared_ptr<RenderPass>> mSubPasses;
		};
	}
}
