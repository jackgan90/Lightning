#pragma once
#include <memory>
#include "container.h"
#include "rendererexportdef.h"
#include "renderconstants.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;

		using RenderTargetID = int;
		class LIGHTNING_RENDER_API IRenderTarget
		{
		public:
			//get pixel sample count
			virtual std::uint32_t GetSampleCount() = 0;
			//get pixel sample quality
			virtual int GetSampleQuality() = 0;
			//get render format
			virtual RenderFormat GetRenderFormat()const = 0;
			//return the attached RT ID
			virtual RenderTargetID GetID()const = 0;
			virtual ~IRenderTarget() = default;
		};
		using SharedRenderTargetPtr = std::shared_ptr<IRenderTarget>;
		using RenderTargetList = Container::Vector<IRenderTarget*>;
	}
}
