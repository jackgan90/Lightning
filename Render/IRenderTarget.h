#pragma once
#include "RenderConstants.h"
#include "Texture/ITexture.h"

namespace Lightning
{
	namespace Render
	{
		using RenderTargetID = int;
		struct IRenderTarget : Plugins::IRefObject
		{
			//return the attached RT ID
			virtual RenderTargetID INTERFACECALL GetID()const = 0;
			virtual ITexture* INTERFACECALL GetTexture()const = 0;
		};
	}
}
