#pragma once
#include "RenderNode.h"

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
			virtual ~RenderPass(){}
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			virtual void Apply() = 0;
			virtual void OnAddRenderNode(const RenderNode& node) = 0;
			virtual void OnFrameBegin() = 0;
			virtual void OnFrameUpdate() = 0;
			RenderPassType GetType()const { return mType; }
		protected:
			RenderPass(RenderPassType type);
			RenderPassType mType;
		};
	}
}
