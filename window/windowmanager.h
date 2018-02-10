#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "singleton.h"
#include "iwindow.h"
#include "windowexportdef.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		class LIGHTNINGGE_WINDOW_API WindowManager
		{
		public:
			WindowManager();
			~WindowManager();
			SharedWindowPtr MakeWindow();
			std::vector<SharedWindowPtr> GetAllWindows()const;
		private:
			std::unordered_map<std::uint32_t, SharedWindowPtr> m_windows;
			std::uint32_t m_currentID;
		};
	}
}
