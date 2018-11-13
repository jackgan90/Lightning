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
#include "tbb/concurrent_vector.h"
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_unordered_map.h"
#include "tbb/concurrent_unordered_set.h"


namespace Lightning
{
	namespace Foundation
	{
		struct Container
		{
			template<typename T>
			using Vector = std::vector<T>;

			template<typename T>
			using ConcurrentVector = tbb::concurrent_vector<T>;

			template<typename T>
			using List = std::list<T>;

			template<typename K, typename V>
			using UnorderedMap = std::unordered_map<K, V>;

			template<typename K, typename V>
			using ConcurrentUnorderedMap = tbb::concurrent_unordered_map<K, V>;

			template<typename K, typename V>
			using Map = std::map<K, V>;

			template<typename T>
			using UnorderedSet = std::unordered_set<T>;

			template<typename T>
			using ConcurrentUnorderedSet = tbb::concurrent_unordered_set<T>;

			template<typename T>
			using Set = std::set<T>;

			template<typename T>
			using Deque = std::deque<T>;

			template<typename T>
			using Queue = std::queue<T>;

			template<typename T,
				typename Container = Vector<T>,
				typename Compare = std::less<typename Container::value_type>>
			using PriorityQueue = std::priority_queue<T, Container, Compare>;

			template<typename T>
			using ConcurrentQueue = tbb::concurrent_queue<T>;

			template<typename T, std::size_t Size>
			using Array = std::array<T, Size>;

			template<typename... Args>
			using Tuple = std::tuple<Args...>;
		};

	}
}