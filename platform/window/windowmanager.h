#pragma once
#include <unordered_map>
#include <vector>
#include "iwindow.h"
#include "platformexportdef.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		using WINDOWID = unsigned int;
		class LIGHTNINGGE_PLATFORM_API WindowManager
		{
		public:
			WindowManager(const WindowManager& wm) = delete;
			WindowManager(WindowManager&& wm) = delete;
			~WindowManager();
			WindowPtr MakeWindow();
			static WindowManager* Instance();
			std::vector<WindowPtr> GetAllWindows()const;
		private:
			WindowManager();
		private:
			std::unordered_map<WINDOWID, WindowPtr> m_windows;
			WINDOWID m_currentID;
		};
	}
}
