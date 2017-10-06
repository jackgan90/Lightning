#pragma once
#include <cstddef>
#include <unordered_map>
#include "utilitiesexportdef.h"

namespace LightningGE
{
	namespace Utility
	{
		template<class Container, class Iterator, class Predicate>
		Iterator erase_if(Container container, Iterator begin, Iterator end, Predicate predicate)
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

		LIGHTNINGGE_UTILITIES_API std::size_t CalculateHash(const std::string& str, const std::unordered_map<std::string, std::string>& map);

	}
}