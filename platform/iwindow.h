#pragma once
#include <memory>
#include "common.h"

namespace LightningGE
{
	namespace WindowSystem
	{
		class DLLEXPORT IWindow
		{
		public:
			friend class WindowManager;
			virtual void destroy() = 0;
			virtual bool Init() = 0;
			virtual bool Show(bool show) = 0;
			virtual ~IWindow(){};
		protected:
			IWindow() {};
		};
		using WindowPtr = std::shared_ptr<IWindow>;
	}
}
