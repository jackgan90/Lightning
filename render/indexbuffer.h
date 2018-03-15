#pragma once
#include <memory>
#include "iindexbuffer.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API IndexBuffer : public IIndexBuffer
		{
		protected:
			IndexBuffer(IndexType type);
		};
	}
}
