#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "singleton.h"
#include "iwindow.h"
#include "windowexportdef.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class LIGHTNING_WINDOW_API WindowManager
		{
		public:
			WindowManager();
			~WindowManager();
			SharedWindowPtr MakeWindow();
			std::vector<SharedWindowPtr> GetAllWindows()const;
		private:
			std::unordered_map<std::uint32_t, SharedWindowPtr> mWindows;
			std::uint32_t mCurrentID;
		};
	}
}
