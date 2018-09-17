#pragma once
#include "rendererexportdef.h"
#include "renderitem.h"

namespace Lightning
{
	namespace Render
	{
		enum class RenderPassType
		{
			FORWARD,
			DEFERED
		};

		class LIGHTNING_RENDER_API RenderPass
		{
		public:
			virtual ~RenderPass(){}
			virtual void Draw(const RenderItem& item) = 0;
			//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
			virtual void Apply() = 0;
			RenderPassType GetType()const { return mType; }
		protected:
			RenderPass(RenderPassType type):mType(type){}
			RenderPassType mType;
		};
	}
}
