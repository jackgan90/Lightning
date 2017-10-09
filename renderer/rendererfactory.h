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

namespace LightningGE
{
	namespace Renderer
	{
		//this class is designed to create singleton render objects.only application and render module should use this class
		template<typename T>
		class LIGHTNINGGE_RENDERER_API RendererFactory : public Foundation::Singleton<RendererFactory<T>>
		{
		private:
#ifdef LIGHTNINGGE_USE_D3D12
			typedef D3D12RenderTargetManager RenderTargetManagerType;
			typedef D3D12RenderContext RenderContextType;
			typedef D3D12Renderer RendererType;
			typedef D3D12ShaderManager ShaderManagerType;
#endif
			template<typename Q, typename... Args>
			std::shared_ptr<Q> CreateInternal(typename std::enable_if<std::is_same<Q, IRenderContext>::value, RenderContextType>::type* t, Args&&... args)
			{
				return CreateInterfaceImpl<Q, RenderContextType>(std::forward<Args>(args)...);
			}
			template<typename Q, typename... Args>
			std::shared_ptr<Q> CreateInternal(typename std::enable_if<std::is_same<Q, IRenderTargetManager>::value, RenderTargetManagerType>::type* t, Args&&... args)
			{
				return CreateInterfaceImpl<Q, RenderTargetManagerType>(std::forward<Args>(args)...);
			}
			template<typename Q, typename... Args>
			std::shared_ptr<Q> CreateInternal(typename std::enable_if<std::is_same<Q, IRenderer>::value, RendererType>::type* t, Args&&... args)
			{
				return CreateInterfaceImpl<Q, RendererType>(std::forward<Args>(args)...);
			}
			template<typename Q, typename... Args>
			std::shared_ptr<Q> CreateInternal(typename std::enable_if<std::is_same<Q, IShaderManager>::value, ShaderManagerType>::type* t, Args&&... args)
			{
				return CreateInterfaceImpl<Q, ShaderManagerType>(std::forward<Args>(args)...);
			}
		public:
			template<typename... Args>
			std::shared_ptr<T> Create(Args&&... args)
			{
				if (m_obj)
				{
					return m_obj;
				}
				m_obj = CreateInternal<T>(nullptr, std::forward<Args>(args)...);
				return m_obj;
			}

			std::shared_ptr<T> Get()const
			{
				return m_obj;
			}

			void Finalize()
			{
				m_obj.reset();
			}
		private:
			template<typename Interface, typename Implementation, typename... Args>
			std::shared_ptr<Interface> CreateInterfaceImpl(Args&&... args)
			{
				return std::make_shared<Implementation>(new Implementation(std::forward<Args>(args)...));
			}
			std::shared_ptr<T> m_obj;
		};
	}
}