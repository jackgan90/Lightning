#pragma once
#include "job.h"
#define USE_CUSTOM_CONCURRENT_QUEUE

#ifndef USE_CUSTOM_CONCURRENT_QUEUE
#include "concurrentqueue.h"
#endif // !USE_CUSTOM_CONCURRENT_QUEUE


namespace JobSystem
{
	class JobQueue
	{
	public:
		explicit JobQueue::JobQueue(std::size_t size)
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			:m_queue(size) 
#else
			:m_tail(0), m_head(0), m_tailAnchor(0), m_blockQueueSize(size / 2)
#endif
		{
#ifdef USE_CUSTOM_CONCURRENT_QUEUE
			for (std::size_t i = 0;i < 2;++i)
			{
				m_blocks.emplace_back(m_blockQueueSize);
				auto& block = m_blocks.back();
				block.start = i * m_blockQueueSize;
				block.end = (i + 1) * m_blockQueueSize;
			}
#endif
		}
		JobQueue(const JobQueue&) = delete;
		JobQueue& operator=(const JobQueue&) = delete;
		void Push(IJob* job)
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			auto res = m_queue.enqueue(job);
			assert(res);
#else
			auto slot = m_tailAnchor.fetch_add(1, std::memory_order_release);
			Assign(slot, job);
			m_tail.fetch_add(1, std::memory_order_relaxed);
#endif
		}
		IJob* Pop()
		{
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
			if (m_queue.size_approx() > 0)
			{
				IJob* job{ nullptr };
				auto success = m_queue.try_dequeue(job);
				if (!success)
					return nullptr;
				return job;
			}
#else
			auto anchor = m_tailAnchor.load(std::memory_order_relaxed);
			if (anchor > 0)
			{
				auto tail = m_tail.load(std::memory_order_relaxed);
				if (tail == anchor)
				{
					auto head = m_head.load(std::memory_order_relaxed);
					if (tail > head)
					{
						auto job = GetJob(head);
						if (m_head.compare_exchange_strong(head, head + 1, std::memory_order_relaxed))
						{
							return job;
						}
					}
				}
			}
#endif
			return nullptr;
		}
	private:
#ifndef USE_CUSTOM_CONCURRENT_QUEUE
		moodycamel::ConcurrentQueue<IJob*> m_queue;
#else
		struct Block
		{
			Block(std::uint32_t size)
			{
				queue = new IJob*[size];
			}
			Block(const Block&) = delete;
			Block& operator=(const Block&) = delete;
			Block(Block&& b):start(b.start), end(b.end), queue(b.queue)
			{
				b.queue = nullptr;
			}
			Block& operator=(Block&& b)
			{
				if (&b != this)
				{
					start = b.start;
					end = b.end;
					queue = b.queue;
					b.queue = nullptr;
				}
				return *this;
			}
			~Block()
			{
				if (queue)
				{
					delete[] queue;
					queue = nullptr;
				}
			}
			std::int64_t start;
			std::int64_t end;
			IJob** queue;
		};
		void Assign(std::int64_t index, IJob* job)
		{
			while (true)
			{
				bool success{ false };
				for (auto& block : m_blocks)
				{
					if (index >= block.start && index < block.end)
					{
						block.queue[index % m_blockQueueSize] = job;
						success = true;
						break;
					}
				}
				if (success)
					break;
				else
				{
					if (index % m_blockQueueSize == 0)
					{
						AddOrRecycleBlock(index);
					}
					else
					{
						std::this_thread::yield();
					}
				}
			}
		}

		IJob* GetJob(std::int64_t index)
		{
			for (auto& block : m_blocks)
			{
				if (index >= block.start && index < block.end)
				{
					auto job = block.queue[index % m_blockQueueSize];
					job->GetType();
					return job;
				}
			}
			return nullptr;
		}

		void AddOrRecycleBlock(std::int64_t index)
		{
			Block* pTargetBlock{ nullptr };
			auto head = m_head.load(std::memory_order_relaxed);
			for (auto it = m_blocks.begin();it != m_blocks.end();)
			{
				if (head >= it->end)
				{
					if(!pTargetBlock)
						pTargetBlock = &(*it);
					else
					{
						if (m_blocks.size() > 2)
						{
							it = m_blocks.erase(it);
							continue;
						}
					}
				}
				++it;
			}
			if (!pTargetBlock)
			{
				m_blocks.emplace_back(m_blockQueueSize);
				auto& block = m_blocks.back();
				block.start = index;
				block.end = index + m_blockQueueSize;
			}
			else
			{
				pTargetBlock->start = index;
				pTargetBlock->end = index + m_blockQueueSize;
			}
		}

		std::atomic<std::int64_t> m_head;
		std::atomic<std::int64_t> m_tail;
		std::atomic<std::int64_t> m_tailAnchor;
		std::uint32_t m_blockQueueSize;
		std::vector<Block> m_blocks;
#endif
	};
}
