#include "Environment.h"

namespace Lightning
{
	namespace Foundation
	{
		std::thread::id Environment::LoaderIOThreadID;

		bool Environment::IsInLoaderIOThread()
		{
			return std::this_thread::get_id() == LoaderIOThreadID;
		}
	}
}