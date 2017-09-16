#pragma once
#include "platformexportdef.h"
#include "iapplication.h"
#include "iwindow.h"

namespace LightningGE
{
	namespace App
	{
		using WindowSystem::WindowPtr;
		class LIGHTNINGGE_PLATFORM_API Win32Application : public IApplication
		{
		public:
			Win32Application();
			~Win32Application()override;
			bool Init()override;
			bool Start()override;
			int Run()override;
			void Quit()override;
		protected:
			WindowPtr m_pWin;
		private:
			void TryDestroyWindow();
		};
	}
}