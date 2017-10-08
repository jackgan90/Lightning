#include "windowmanager.h"
#ifdef LIGHTNINGGE_WIN32
#include "winwindow.h"
#endif

namespace LightningGE
{
	namespace WindowSystem
	{
		WindowManager::WindowManager():m_currentID(0)
		{
			
		}

		WindowPtr WindowManager::MakeWindow()
		{
		#ifdef LIGHTNINGGE_WIN32
			m_windows.insert(std::make_pair(m_currentID++, WindowPtr(new WinWindow())));
			return m_windows[m_currentID-1];
		#endif
			return WindowPtr();
		}

		std::vector<WindowPtr> WindowManager::GetAllWindows()const
		{
			std::vector<WindowPtr> windows;
			for (auto w : m_windows)
				windows.push_back(w.second);
			return windows;
		}

		WindowManager::~WindowManager()
		{
			for (auto w : m_windows)
				w.second->destroy();
			m_windows.clear();
		}
	}
}
