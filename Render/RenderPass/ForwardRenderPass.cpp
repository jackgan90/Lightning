#include "ForwardRenderPass.h"
#include "tbb/flow_graph.h"
#include "tbb/parallel_for.h"


namespace Lightning
{
	namespace Render
	{
		//Apply is called by renderer once per frame.Subclasses should commit render resources to device in this method.
		void ForwardRenderPass::Apply(DrawCommandQueue& q)
		{
			tbb::parallel_for(tbb::blocked_range<std::size_t>(0, q.size()), 
				[&q, this](const tbb::blocked_range<std::size_t>& range) {
				for (std::size_t i = range.begin(); i != range.end();++i)
				{
					q[i]->Apply();
				}
			});
		}
	}
}