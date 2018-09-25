#pragma once
#include <cstdint>
#include "container.h"
#include "singleton.h"
#include "iwindow.h"
#include "windowexportdef.h"

namespace Lightning
{
	namespace WindowSystem
	{
		using Foundation::container;

		class LIGHTNING_WINDOW_API WindowManager
		{
		public:
			WindowManager();
			~WindowManager();
			SharedWindowPtr MakeWindow();
			container::vector<SharedWindowPtr> GetAllWindows()const;
		private:
			container::unordered_map<std::uint32_t, SharedWindowPtr> mWindows;
			std::uint32_t mCurrentID;
		};
	}
}
