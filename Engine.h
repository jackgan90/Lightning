#pragma once
#include <memory>

namespace Lightning
{
	//Engine is designed such that it depends on no other module
	class Engine
	{
	public:
		static Engine* Instance()
		{
			static Engine sInstance;
			return &sInstance;
		}
		int Run();
	private:
		Engine();
	};
}
