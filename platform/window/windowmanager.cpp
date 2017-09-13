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
			m_windows.insert(std::make_pair(m_currentID++, std::make_shared<WinWindow>()));
			return m_windows[m_currentID-1];
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
