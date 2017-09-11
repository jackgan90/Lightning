#pragma once
#include <unordered_map>
#include "common.h"
#include "iwindow.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		using WINDOWID = unsigned int;
		class DLLEXPORT WindowManager
		{
		public:
			WindowManager(const WindowManager& wm) = delete;
			WindowManager(WindowManager&& wm) = delete;
			~WindowManager();
			WindowPtr MakeWindow();
			static WindowManager* Instance();
		private:
			WindowManager();
		private:
			std::unordered_map<WINDOWID, WindowPtr> m_windows;
			WINDOWID m_currentID;
		};
	}
}
