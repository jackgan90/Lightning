#pragma once
#include "RenderConstants.h"
#include "IRefObject.h"

namespace Lightning
{
	namespace Render
	{
		using RenderTargetID = int;
		struct IRenderTarget : Plugins::IRefObject
		{
			//get pixel sample count
			virtual std::uint32_t INTERFACECALL GetSampleCount() = 0;
			//get pixel sample quality
			virtual int INTERFACECALL GetSampleQuality() = 0;
			//get render format
			virtual RenderFormat INTERFACECALL GetRenderFormat()const = 0;
			//return the attached RT ID
			virtual RenderTargetID INTERFACECALL GetID()const = 0;
		};
	}
}
