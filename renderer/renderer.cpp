#include "renderer.h"

namespace LightningGE
{
	namespace Renderer
	{
		Renderer::Renderer(const SharedFileSystemPtr& fs) :m_frameIndex(0), m_fs(fs)
		{

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