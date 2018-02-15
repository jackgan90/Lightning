#pragma once
#include <memory>
#include "entitiesexportdef.h"
#include "renderer.h"

namespace LightningGE
{
	namespace Entities
	{
		using Render::Renderer;
		class LIGHTNINGGE_ENTITIES_API IDrawable
		{
		public:
			virtual ~IDrawable() = default;
			virtual void Draw(Renderer& renderer) = 0;
		};
		using SharedDrawablePtr = std::shared_ptr<IDrawable>;
	}
}