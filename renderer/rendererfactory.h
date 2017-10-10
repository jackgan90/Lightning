#pragma once
#include <memory>
#include "common.h"
#include "singleton.h"
#include "rendererexportdef.h"
#include "idevice.h"
#include "iswapchain.h"
#include "d3d12rendercontext.h"
#include "d3d12rendertargetmanager.h"
#include "d3d12shadermanager.h"
#include "d3d12renderer.h"

#define DECLARE_RENDERER_FACTORY_PRODUCT(Interface, Implementation) \
	typedef Implementation Implementation##Type; \
	template<typename Q, typename... Args> \
	typename std::enable_if<std::is_same<Q, Interface>::value, Q>::type* CreateInternal(Args&&... args) \
	{ \
		return CreateInterfaceImpl<Implementation##Type>(std::forward<Args>(args)...); \
	} \


namespace LightningGE
{
	namespace Renderer
	{
		//this class is designed to create singleton render objects.only application and render module should use this class
		template<typename Interface>
		class RendererFactory : public Foundation::Singleton<RendererFactory<Interface>>
		{
		private:
#ifdef LIGHTNINGGE_USE_D3D12
			DECLARE_RENDERER_FACTORY_PRODUCT(IRenderTargetManager, D3D12RenderTargetManager)
			DECLARE_RENDERER_FACTORY_PRODUCT(IRenderContext, D3D12RenderContext)
			DECLARE_RENDERER_FACTORY_PRODUCT(IRenderer, D3D12Renderer)
			DECLARE_RENDERER_FACTORY_PRODUCT(IShaderManager, D3D12ShaderManager)
#endif
		public:
			template<typename... Args>
			Interface* Create(Args&&... args)
			{
				if (m_obj)
				{
					return m_obj;
				}
				m_obj = CreateInternal<Interface>(std::forward<Args>(args)...);
				return m_obj;
			}

			Interface* Get()const
			{
				return m_obj;
			}

			void Finalize()
			{
				SAFE_DELETE(m_obj);
			}
		private:
			template<typename Implementation, typename... Args>
			Interface* CreateInterfaceImpl(Args&&... args)
			{
				return new Implementation(std::forward<Args>(args)...);
			}
			Interface* m_obj;
		};
	}
}