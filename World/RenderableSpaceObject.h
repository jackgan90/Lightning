#pragma once
#include "SpaceObject.h"
#include "IRenderable.h"

namespace Lightning
{
	namespace World
	{
		template<typename Interface, typename Implementation>
		class RenderableSpaceObject : public Interface, public SpaceObject<Implementation>
		{
			static_assert(std::is_base_of<IRenderable, Interface>::value, "Interface must be a subclass of IRenderable.");
		public:
			RenderableSpaceObject() : mRenderResourceDirty(true){}
			bool NeedRender()const override { return true; }
			const Transform GetDrawTransform()const override { return GetGlobalTransform(); }
			std::shared_ptr<Render::IIndexBuffer> GetIndexBuffer()const override { return mIndexBuffer; }
			const std::vector<std::shared_ptr<Render::IVertexBuffer>>& GetVertexBuffers()const override
			{
				return mVertexBuffers;
			}
			std::shared_ptr<Render::IMaterial> GetMaterial()const override { return mMaterial; }
			void Render(Render::IRenderer& renderer, const std::shared_ptr<Render::ICamera>& camera)override
			{
				if (!NeedRender())
					return;
				if (mRenderResourceDirty)
				{
					mRenderResourceDirty = false;
					UpdateRenderResources();
				}
				renderer.Draw(shared_from_this(), camera);
			}
		protected:
			virtual void UpdateRenderResources() = 0;
			std::shared_ptr<Render::IIndexBuffer> mIndexBuffer;
			std::vector<std::shared_ptr<Render::IVertexBuffer>> mVertexBuffers;
			std::shared_ptr<Render::IMaterial> mMaterial;
			bool mRenderResourceDirty;
		};
	}
}
