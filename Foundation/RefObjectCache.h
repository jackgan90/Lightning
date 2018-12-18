#pragma once
#include <mutex>
#include <string>
#include "Container.h"
#include "Singleton.h"
#include "RefCount.h"

namespace Lightning
{
	namespace Foundation
	{
		template<typename T>
		class RefObjectCache : public Singleton<RefObjectCache<T>>
		{
			static_assert(std::is_base_of<Plugins::RefCount, T>::value, "T must be a subclass of RefCount.");
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

			T* GetObject(const std::string& name)
			{
				std::lock_guard<std::mutex> lock(mObjectMutex);
				auto it = mObjects.find(name);
				if (it != mObjects.end())
					return it->second;
				return nullptr;
			}

			bool AddObject(const std::string& name, T* object)
			{
				assert(object != nullptr && "Try to cache a null object!");
				std::lock_guard<std::mutex> lock(mObjectMutex);
				if (mObjects.find(name) == mObjects.end())
				{
					object->AddRef();
					mObjects.emplace(name, object);
					return true;
				}
				return false;
			}
			bool RemoveObject(const std::string& name)
			{
				std::lock_guard<std::mutex> lock(mObjectMutex);
				auto it = mObjects.find(name);
				if (it != mObjects.end())
				{
					it->second->Release();
					mObjects.erase(it);
					return true;
				}
				return false;
			}
		protected:
			friend class Singleton<RefObjectCache>;
			RefObjectCache(){}
			Container::UnorderedMap<std::string, T*> mObjects;
			std::mutex mObjectMutex;
		};
	}
}