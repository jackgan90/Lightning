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
			assert(s_instance);
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

		void Renderer::Draw(const SharedGeometryPtr& geometry, const SharedMaterialPtr& material)
		{
			if (m_renderPass)
				m_renderPass->Draw(geometry, material);
		}

		void Renderer::SetRenderPass(RenderPassType type)
		{
			m_renderPass.reset();
			switch (type)
			{
			case LightningGE::Render::RENDERPASS_FORWARD:
				m_renderPass = std::make_unique<ForwardRenderPass>();
				break;
			case LightningGE::Render::RENDERPASS_DEFERED:
				m_renderPass = std::make_unique<DeferedRenderPass>();
				break;
			default:
				break;
			}
		}

	}
}