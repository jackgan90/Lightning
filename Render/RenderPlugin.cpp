#include "RenderPlugin.h"
#include "Material.h"
#include "Renderer.h"
#include "RendererFactory.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Render;
		class RenderPluginImpl : public RenderPlugin
		{
		public:
			RenderPluginImpl(IPluginMgr*):mRenderer(nullptr){}
			~RenderPluginImpl()override;
			IMaterial* CreateMaterial()override;
			IRenderer* GetRenderer()override;
			Render::IRenderer* CreateRenderer(Window::IWindow*)override;
			void DestroyRenderer(Render::IRenderer*)override;
		private:
			IRenderer* mRenderer;
			void DestroyRendererImpl(Render::IRenderer*);
		};

		RenderPluginImpl::~RenderPluginImpl()
		{
			DestroyRendererImpl(mRenderer);
		}

		IMaterial* RenderPluginImpl::CreateMaterial()
		{
			return new Material();
		}

		IRenderer* RenderPluginImpl::GetRenderer()
		{
			return mRenderer;
		}

		IRenderer* RenderPluginImpl::CreateRenderer(Window::IWindow* window)
		{
			assert(mRenderer == nullptr && "There already exists an instance of IRenderer.");
			mRenderer = RendererFactory::Instance()->CreateRenderer(window);
			return mRenderer;
		}

		void RenderPluginImpl::DestroyRenderer(Render::IRenderer* renderer)
		{
			DestroyRendererImpl(renderer);
		}

		void RenderPluginImpl::DestroyRendererImpl(Render::IRenderer* renderer)
		{
			assert(mRenderer == renderer && "Renderer mismatched!");
			if (renderer)
			{
				delete renderer;
				mRenderer = nullptr;
			}
		}
	}
}

LIGHTNING_PLUGIN_INTERFACE(RenderPluginImpl)
