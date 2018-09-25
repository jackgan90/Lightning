#pragma once
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <array>
#include <deque>
#include <queue>
#include <tuple>


namespace Lightning
{
	namespace Foundation
	{
		struct container
		{
			template<typename T>
			using vector = std::vector<T>;

			template<typename T>
			using list = std::list<T>;

			template<typename K, typename V>
			using unordered_map = std::unordered_map<K, V>;

			template<typename K, typename V>
			using map = std::map<K, V>;

			template<typename T>
			using unordered_set = std::unordered_set<T>;

			template<typename T>
			using set = std::set<T>;

			template<typename T>
			using deque = std::deque<T>;

			template<typename T>
			using queue = std::queue<T>;

			template<typename T, std::size_t Size>
			using array = std::array<T, Size>;

			template<typename... Args>
			using tuple = std::tuple<Args...>;
		};

	}
}