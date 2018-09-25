#pragma once
#include <cstddef>
#include "container.h"
#include "utilitiesexportdef.h"

namespace Lightning
{
	namespace Utility
	{
		using Foundation::container;
		template<class container, class Iterator, class Predicate>
		Iterator erase_if(container container, Iterator begin, Iterator end, Predicate predicate)
		{
			Iterator it = begin;
			while (it != end)
			{
				if (predicate(*it))
				{
					it = container.erase(it);
				}
				else
				{
					++it;
				}
			}
			return it;
		}

		LIGHTNING_UTILITIES_API std::size_t CalculateHash(const std::string& str, const container::unordered_map<std::string, std::string>& map);

	}
}
