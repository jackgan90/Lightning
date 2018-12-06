#include "RenderPlugin.h"
#include "Material.h"
#include "Renderer.h"
#include "RendererFactory.h"
#include "IPluginMgr.h"
#include "Logger.h"
#include "FoundationPlugin.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Render;
		class RenderPluginImpl : public RenderPlugin
		{
		public:
			RenderPluginImpl();
			~RenderPluginImpl()override;
			IMaterial* CreateMaterial()override;
			IRenderer* GetRenderer()override;
			Render::IRenderer* CreateRenderer(Window::IWindow*)override;
			void DestroyRenderer(Render::IRenderer*)override;
			void Update()override;
		protected:
			void OnCreated(IPluginMgr*)override;
		private:
			void DestroyRendererImpl(Render::IRenderer*);
			IPluginMgr* mPluginMgr;
			IRenderer* mRenderer;
		};

		RenderPluginImpl::RenderPluginImpl():mRenderer(nullptr)
		{
		}

		void RenderPluginImpl::OnCreated(IPluginMgr* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Render)
			LOG_INFO("Render plugin init.");
		}

		RenderPluginImpl::~RenderPluginImpl()
		{
			LOG_INFO("Render plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
			DestroyRendererImpl(mRenderer);
		}

		void RenderPluginImpl::Update()
		{
			mRenderer->Render();
			
		}

		IMaterial* RenderPluginImpl::CreateMaterial()
		{
			return NEW_REF_OBJ(Material);
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

LIGHTNING_PLUGIN_IMPL(RenderPluginImpl)
