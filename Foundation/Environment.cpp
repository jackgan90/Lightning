#include "Environment.h"

namespace Lightning
{
	namespace Foundation
	{
		void Environment::SetLoaderIOThreadID(std::thread::id id)
		{
			mLoaderIOThreadID = id;
		}

		bool Environment::IsInLoaderIOThread()const
		{
			return std::this_thread::get_id() == mLoaderIOThreadID;
		}
	}
}