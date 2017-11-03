#include <cassert>
#include "renderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		Renderer* Renderer::s_instance = nullptr;
		Renderer::Renderer(const SharedFileSystemPtr& fs) :m_frameIndex(0), m_fs(fs)
		{
			assert(s_instance == nullptr);
			s_instance = this;
		}

		Renderer::~Renderer()
		{
			assert(s_instance != nullptr);
			s_instance = nullptr;
		}

		void Renderer::Render()
		{
			BeginRender();
			DoRender();
			EndRender();
		}

		unsigned long Renderer::GetCurrentFrameIndex()const
		{
			return m_frameIndex;
		}

		IDevice* Renderer::GetDevice()
		{
			return m_device.get();
		}

	}
}