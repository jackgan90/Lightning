#pragma once
#include <unordered_map>
#include <vector>
#include "singleton.h"
#include "iwindow.h"
#include "windowexportdef.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		typedef unsigned int WINDOWID;
		class LIGHTNINGGE_WINDOW_API WindowManager
		{
		public:
			WindowManager();
			~WindowManager();
			SharedWindowPtr MakeWindow();
			std::vector<SharedWindowPtr> GetAllWindows()const;
		private:
			std::unordered_map<WINDOWID, SharedWindowPtr> m_windows;
			WINDOWID m_currentID;
		};
	}
}
