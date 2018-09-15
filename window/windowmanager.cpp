#include "windowmanager.h"
#ifdef LIGHTNING_WIN32
#include "winwindow.h"
#endif

namespace Lightning
{
	namespace WindowSystem
	{
		WindowManager::WindowManager():m_currentID(0)
		{
			
		}

		SharedWindowPtr WindowManager::MakeWindow()
		{
		#ifdef LIGHTNING_WIN32
			m_windows.insert(std::make_pair(m_currentID++, SharedWindowPtr(new WinWindow())));
			return m_windows[m_currentID-1];
		#endif
			return SharedWindowPtr();
		}

		std::vector<SharedWindowPtr> WindowManager::GetAllWindows()const
		{
			std::vector<SharedWindowPtr> windows;
			for (auto w : m_windows)
				windows.push_back(w.second);
			return windows;
		}

		WindowManager::~WindowManager()
		{
		}
	}
}
