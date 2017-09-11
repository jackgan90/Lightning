#include "windowmanager.h"
#ifdef LIGHTNINGGE_WIN32
#include "winwindow.h"
#endif

namespace LightningGE
{
	namespace WindowSystem
	{

		WindowManager* WindowManager::Instance()
		{
			static WindowManager instance;
			return &instance;
		}

		WindowManager::WindowManager():m_currentID(0)
		{
			
		}

		WindowPtr WindowManager::MakeWindow()
		{
		#ifdef LIGHTNINGGE_WIN32
			WindowPtr pWindow = std::make_shared<WinWindow>(WinWindow());
			m_windows[m_currentID++] = pWindow;
			return pWindow;
		#endif
			return WindowPtr();
		}

		WindowManager::~WindowManager()
		{
			for (auto w : m_windows)
				w.second->destroy();
			m_windows.clear();
		}
	}
}
