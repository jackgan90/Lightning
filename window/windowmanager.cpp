#include "windowmanager.h"
#ifdef LIGHTNING_WIN32
#include "winwindow.h"
#endif

namespace Lightning
{
	namespace WindowSystem
	{
		WindowManager::WindowManager():mCurrentID(0)
		{
			
		}

		SharedWindowPtr WindowManager::MakeWindow()
		{
		#ifdef LIGHTNING_WIN32
			mWindows.insert(std::make_pair(mCurrentID++, SharedWindowPtr(new WinWindow())));
			return mWindows[mCurrentID-1];
		#endif
			return SharedWindowPtr();
		}

		std::vector<SharedWindowPtr> WindowManager::GetAllWindows()const
		{
			std::vector<SharedWindowPtr> windows;
			for (auto w : mWindows)
				windows.push_back(w.second);
			return windows;
		}

		WindowManager::~WindowManager()
		{
		}
	}
}
