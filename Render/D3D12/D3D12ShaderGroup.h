#pragma once
#include "Container.h"
#include "ThreadLocalObject.h"
#include "D3D12Shader.h"
#include "D3D12DescriptorHeapManager.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		class D3D12ShaderGroup : boost::noncopyable
		{
		public:
			D3D12ShaderGroup();
			~D3D12ShaderGroup();
			//Add a shader to this group,thread unsafe
			void AddShader(D3D12Shader* shader);
			//Commit all shaders managed by this group.Basically just commit all shader parameters.
			void Commit();
		private:
			//Thread unsafe
			void Destroy();
		private:
			//This struct exists because a ThreadLocalObject is default constructed and lazy initialize.
			//And have to ensure the initial value of heap is nullptr
			struct DescriptorHeapArray
			{
				DescriptorHeapArray()
				{
					std::memset(heaps, 0, sizeof(heaps));
				}
				DescriptorHeap* heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
				//heapList is a temporary vector which only exists to improve performance
				Container::Vector<ID3D12DescriptorHeap*> heapList;
			};
			Container::Vector<D3D12Shader*> mShaders;
			Foundation::ThreadLocalObject<DescriptorHeapArray> mDescriptorHeaps[RENDER_FRAME_COUNT];
			std::size_t mConstantBufferCount;
			std::size_t mTextureCount;
			std::size_t mSamplerStateCount;
		};
	}
}

