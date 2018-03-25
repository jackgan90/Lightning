#include "workstealqueue.h"

namespace JobSystem
{
	WorkStealQueue::WorkStealQueue():
		m_head(0), m_rear(0)
	{

	}

	void WorkStealQueue::Push(IJob* job)
	{
		//TODO : store job in m_jobs may overwrite previous job which may not complete now
		//should deal with this situation later
	}

	IJob* WorkStealQueue::Pop()
	{
		return nullptr;
	}

	IJob* WorkStealQueue::Steal()
	{
		return nullptr;
	}
}