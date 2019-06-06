#pragma once
#include "IRenderable.h"
#include "IDrawable.h"
#include "SpaceObject.h"

namespace Lightning
{
	namespace World
	{
		template<typename Interface, typename Implementation>
		class RenderableSpaceObject : public Interface, public SpaceObject<Implementation>
		{
			static_assert(std::is_base_of<IRenderable, Interface>::value, "Interface must be a subclass of IRenderable.");
		public:
			RenderableSpaceObject(){}
			void Render(Render::IRenderer& renderer, const std::shared_ptr<Render::ICamera>& camera)override
			{
				for (const auto& renderResource : mRenderResources)
				{
					renderer.Draw(renderResource, camera);
				}
			}
		protected:
			using Super = RenderableSpaceObject<Interface, Implementation>;
			struct RenderResource : Render::IDrawable
			{
				Render::PrimitiveType GetPrimitiveType()const override { return primitiveType; }
				std::shared_ptr<Render::IIndexBuffer> GetIndexBuffer()const override{ return indexBuffer; }
				const std::vector<std::shared_ptr<Render::IVertexBuffer>>& GetVertexBuffers()const override { return vertexBuffers; }
				std::shared_ptr<Render::IMaterial> GetMaterial()const override { return material; }
				const Transform GetTransform()const override { return globalTransform; }

				std::shared_ptr<Render::IIndexBuffer> indexBuffer;
				std::vector<std::shared_ptr<Render::IVertexBuffer>> vertexBuffers;
				std::shared_ptr<Render::IMaterial> material;
				Render::PrimitiveType primitiveType;
				Transform globalTransform;
			};
			std::vector<std::shared_ptr<RenderResource>> mRenderResources;
		};
	}
}
