#include "IRenderPlugin.h"
#include "Material.h"
#include "Renderer.h"
#include "RendererFactory.h"
#include "IPluginManager.h"
#include "Logger.h"
#include "IFoundationPlugin.h"
#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		using namespace Render;
		class RenderPluginImpl : public IRenderPlugin
		{
		public:
			RenderPluginImpl();
			INTERFACECALL ~RenderPluginImpl()override;
			IMaterial* INTERFACECALL CreateMaterial()override;
			IRenderer* INTERFACECALL GetRenderer()override;
			Render::IRenderer* INTERFACECALL CreateRenderer(Window::IWindow*)override;
			void INTERFACECALL DestroyRenderer(Render::IRenderer*)override;
			void INTERFACECALL Update()override;
			void INTERFACECALL OnCreated(IPluginManager*)override;
		private:
			void DestroyRendererImpl(Render::IRenderer*);
			IPluginManager* mPluginMgr;
			IRenderer* mRenderer;
			PLUGIN_OVERRIDE(RenderPluginImpl)
		};

		RenderPluginImpl::RenderPluginImpl():mRenderer(nullptr)
		{
		}

		void RenderPluginImpl::OnCreated(IPluginManager* mgr)
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
