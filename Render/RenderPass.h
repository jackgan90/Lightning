#pragma once
#include "IDrawCommand.h"

namespace Lightning
{
	namespace Render
	{
		enum class RenderPassType
		{
			FORWARD,
			DEFERED
		};

		class RenderPass
		{
		public:
			virtual ~RenderPass() = default;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			virtual void Apply(DrawCommandQueue&) = 0;
			virtual void OnFrameEnd() = 0;
			RenderPassType GetType()const { return mType; }
		protected:
			RenderPass(RenderPassType type);
			RenderPassType mType;
		};
	}
}
