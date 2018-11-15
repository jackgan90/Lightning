#include "Plugin.h"

namespace Lightning
{
	namespace Plugins
	{
		Plugin::Plugin(const std::string& name): mName(name), mRefCount(1)
		{

		}

		void Plugin::AddRef()
		{
			mRefCount.fetch_add(1, std::memory_order_relaxed);
		}

		bool Plugin::Release()
		{
			auto oldRefCount = mRefCount.fetch_sub(1, std::memory_order_relaxed);
			if (oldRefCount == 1)
			{
				delete this;
				return true;
			}
			return false;
		}

		std::string Plugin::GetFullName()const
		{
			return mName + PluginExtension;
		}
	}
}