#include "Environment.h"

namespace Lightning
{
	namespace Foundation
	{
		void Environment::InitLoaderIOThreadID()
		{
			mLoaderIOThreadID = std::this_thread::get_id();
		}

		bool Environment::IsInLoaderIOThread()const
		{
			return std::this_thread::get_id() == mLoaderIOThreadID;
		}
	}
}