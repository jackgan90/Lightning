#include <iostream>
#include <random>
#include "catch.hpp"
#include "ecs/entitymanager.h"
#include "ecs/systemmanager.h"
#include "ecs/event.h"

namespace
{
	TEST_CASE("ECSTest")
	{
		using namespace Lightning::Foundation;
		bool running{ true };
		class TestEntity : public Entity
		{

		};

		class Comp1 : public Component
		{
		public:
			Comp1(int a, char b) : _a(a), _b(b)
			{
				std::cout << "Comp1 construct.a = " << a << ", b = " << b << std::endl;
			}
			int _a;
			char _b;
		};

		class Comp2 : public Component
		{
		public:
			Comp2()
			{
				std::cout << "Comp2 construct." << std::endl;
			}
		};

		class System1 : public System
		{
		public:
			System1(bool *pRunning) :System(2), mRunning(pRunning){}
			int updateCount{ 0 };
			bool* mRunning;
			void Update(const EntityPtr& entity)override
			{
				std::cout << "System1 Update" << std::endl;
				updateCount++;
				if (updateCount > 5000)
					*mRunning = false;
			}
		};

		class System2 : public System
		{
		public:
			bool* mRunning;
			System2(bool* pRunning) :System(0), mRunning(pRunning){
				SystemManager::Instance()->CreateSystem<System1>(mRunning);
			}
			void Update(const EntityPtr& entity)override
			{
				std::cout << "System2 Update" << std::endl;
			}
		};

		class System3 : public System
		{
		public:
			bool* mRunning;
			System3(bool* pRunning) :System(5), mRunning(pRunning){}
			bool system2Created{ false };
			container::vector<EntityID> mEntities;
			void Update(const EntityPtr& entity)override
			{
				std::cout << "System3 Update" << std::endl;
				static std::size_t counter{ 0 };
				if (!system2Created)
				{
					SystemManager::Instance()->CreateSystem<System2>(mRunning);
					system2Created = true;
				}
				if (mEntities.size() < 50)
				{
					auto entity = EntityManager::Instance()->CreateEntity<TestEntity>();
					mEntities.push_back(entity->GetID());
					std::cout << "Create a test entity!" << std::endl;
					entity->AddComponent<Comp2>();
					entity->AddComponent<Comp1>(mEntities.size(), 'l');
				}
				else
				{
					for (const auto& id : mEntities)
					{
						auto entity = EntityManager::Instance()->GetEntity<TestEntity>(id);
						auto comp = entity->GetComponent<Comp1>();
						std::cout << "CHange comp1 values : " << std::endl;
						comp->_a = 1024;
						comp->_b = 'a';
						entity->RemoveComponent<Comp1>();
						entity->RemoveComponent<Comp2>();
					}
					mEntities.clear();
				}
			}
		};

		SystemManager::Instance()->CreateSystem<System3>(&running);
		auto entity = EntityManager::Instance()->CreateEntity<TestEntity>();
		entity->AddComponent<Comp1>(10, 'k');

		while (running)
		{
			EntityManager::Instance()->Update();
		}
		int a = 10;
		EntityManager::Instance()->RemoveEntity<TestEntity>(entity->GetID());
	}
}