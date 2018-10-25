#include "d3d12statefulresourcemgr.h"
#include "tbb/spin_mutex.h"

namespace
{
	static tbb::spin_mutex mtxResources;
}

namespace Lightning
{
	namespace Render
	{
		void D3D12StatefulResourceMgr::Notify(ID3D12GraphicsCommandList* cmdList, D3D12StatefulResource* resource)
		{
			tbb::spin_mutex::scoped_lock lock(mtxResources);
			mCmdListResources[cmdList].insert(resource);
		}
	}
}