#include <cassert>
#include "renderer.h"
#include "forwardrenderpass.h"
#include "deferedrenderpass.h"

namespace LightningGE
{
	namespace Render
	{
		Renderer* Renderer::s_instance{ nullptr };
		Renderer::Renderer(const SharedFileSystemPtr& fs, RenderPassType renderPassType) :m_frameIndex(0), m_fs(fs)
		{
			assert(!s_instance);
			s_instance = this;
			SetRenderPass(renderPassType);
		}

		Renderer::~Renderer()
		{
			assert(s_instance == this);
			s_instance = nullptr;
		}

		void Renderer::Render()
		{
			BeginFrame();
			DoFrame();
			ApplyRenderPass();
			EndFrame();
		}

		void Renderer::ApplyRenderPass()
		{
			if (m_renderPass)
				m_renderPass->Apply();
		}

		std::uint64_t Renderer::GetCurrentFrameIndex()const
		{
			return m_frameIndex;
		}

		IDevice* Renderer::GetDevice()
		{
			return m_device.get();
		}

		ISwapChain* Renderer::GetSwapChain()
		{
			return m_swapChain.get();
		}


		void Renderer::Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material, const Transform& transform)
		{
			if (m_renderPass)
				m_renderPass->Draw(geometry, material, transform);
		}

		void Renderer::SetRenderPass(RenderPassType type)
		{
			m_renderPass.reset();
			switch (type)
			{
			case RenderPassType::FORWARD:
				m_renderPass = std::make_unique<ForwardRenderPass>();
				break;
			case RenderPassType::DEFERED:
				m_renderPass = std::make_unique<DeferedRenderPass>();
				break;
			default:
				break;
			}
		}

	}
}