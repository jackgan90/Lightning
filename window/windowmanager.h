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
		class LIGHTNINGGE_WINDOW_API WindowManager : public Singleton<WindowManager>
		{
		public:
			friend class Singleton<WindowManager>;
			~WindowManager();
			WindowPtr MakeWindow();
			std::vector<WindowPtr> GetAllWindows()const;
		private:
			WindowManager();
		private:
			std::unordered_map<WINDOWID, WindowPtr> m_windows;
			WINDOWID m_currentID;
		};
	}
}
