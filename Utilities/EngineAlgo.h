#pragma once
#include <cstddef>
#include "Container.h"
#include "UtilitiesExportDef.h"

namespace Lightning
{
	namespace Utility
	{
		using Foundation::Container;
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

		LIGHTNING_UTILITIES_API std::size_t CalculateHash(const std::string& str, const Container::UnorderedMap<std::string, std::string>& map);

	}
}
