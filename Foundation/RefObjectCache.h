#pragma once
#include <cassert>
#include <mutex>
#include <string>
#include <unordered_map>
#include "Singleton.h"
#include "IRefObject.h"

namespace Lightning
{
	namespace Foundation
	{
		template<typename Derived, typename KeyType, typename ObjectType>
		class RefObjectCache : public Singleton<Derived>
		{
			static_assert(std::is_base_of<Plugins::IRefObject, ObjectType>::value, "T must be a subclass of IRefObject.");
		public:
			virtual ~RefObjectCache()
			{
				Clear();
			}

			void Clear()
			{
				std::lock_guard<std::mutex> lock(mObjectMutex);
				for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
				{
					it->second->Release();
				}
				mObjects.clear();
			}

			ObjectType* GetObject(const KeyType& key)
			{
				std::lock_guard<std::mutex> lock(mObjectMutex);
				auto it = mObjects.find(key);
				if (it != mObjects.end())
					return it->second;
				return nullptr;
			}

			bool AddObject(const KeyType& key, ObjectType* object)
			{
				assert(object != nullptr && "Try to cache a null object!");
				std::lock_guard<std::mutex> lock(mObjectMutex);
				if (mObjects.find(key) == mObjects.end())
				{
					object->AddRef();
					mObjects.emplace(key, object);
					return true;
				}
				return false;
			}
			bool RemoveObject(const KeyType& key)
			{
				std::lock_guard<std::mutex> lock(mObjectMutex);
				auto it = mObjects.find(key);
				if (it != mObjects.end())
				{
					it->second->Release();
					mObjects.erase(it);
					return true;
				}
				return false;
			}
		protected:
			friend class Singleton<Derived>;
			RefObjectCache(){}
			std::unordered_map<KeyType, ObjectType*> mObjects;
			std::mutex mObjectMutex;
		};
	}
}