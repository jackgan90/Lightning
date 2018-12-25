project(Render)
message(STATUS "Configuring project Render")


set(HEADERS IDevice.h
			Device.h
			ISwapChain.h
			IRenderTarget.h
			IDepthStencilBuffer.h
			IRenderer.h
			IRenderFence.h
			Renderer.h
			PipelineState.h
			Semantics.h
			IVertexBuffer.h
			VertexBuffer.h
			IIndexBuffer.h
			IndexBuffer.h
			RendererHelper.h
			RenderException.h
			RendererFactory.h
			IGPUBuffer.h
			RenderConstants.h
			Geometry.h
			IMaterial.h
			Material.h
			RenderPass.h
			ForwardRenderPass.h
			DeferedRenderPass.h
			FrameMemoryAllocator.h
			RenderUnit.h
			RenderObjectCache.h)
set(SOURCES Renderer.cpp
			Device.cpp
			VertexBuffer.cpp
			IndexBuffer.cpp
			RendererHelper.cpp
			RendererFactory.cpp
			Geometry.cpp
			Material.cpp
			RenderPass.cpp
			ForwardRenderPass.cpp
			FrameMemoryAllocator.cpp
			DeferedRenderPass.cpp
			RenderObjectCache.cpp)

set(TYPES_HEADERS	Types/Color.h
					Types/Rect.h)

set(SHADER_HEADERS	Shader/IShader.h
					Shader/Shader.h
					Shader/IShaderMacros.h
					Shader/IShaderParameter.h
					Shader/ShaderParameter.h
					Shader/ShaderMacros.h)
set(SHADER_SOURCES	Shader/ShaderMacros.cpp
					Shader/Shader.cpp)

set(TEXTURE_HEADERS	Texture/ITexture.h
					Texture/Sampler.h)

set(SERIALIZERS_HEADERS	Serializers/ShaderSerializer.h
						Serializers/TextureSerializer.h)

set(SERIALIZERS_SOURCES	Serializers/ShaderSerializer.cpp
						Serializers/TextureSerializer.cpp)

set(UTILITY_HEADERS RenderAllocator.h)
set(UTILITY_SOURCES RenderAllocator.cpp)

set(PLUGIN_HEADERS	IRenderPlugin.h)
set(PLUGIN_SOURCES	RenderPluginImpl.cpp)

list(APPEND HEADERS ${TYPES_HEADERS} ${SHADER_HEADERS} ${UTILITY_HEADERS} ${SERIALIZERS_HEADERS} ${TEXTURE_HEADERS} ${PLUGIN_HEADERS})
list(APPEND SOURCES ${SHADER_SOURCES} ${UTILITY_SOURCES} ${SERIALIZERS_SOURCES} ${PLUGIN_SOURCES})

source_group("Types" FILES ${TYPES_HEADERS})
source_group("Shader" FILES ${SHADER_HEADERS}
							${SHADER_SOURCES})

source_group("Texture" FILES ${TEXTURE_HEADERS})

source_group("Utility" FILES ${UTILITY_HEADERS} ${UTILITY_SOURCES})

source_group("Serializers" FILES ${SERIALIZERS_HEADERS} ${SERIALIZERS_SOURCES})

source_group("Plugin" FILES ${PLUGIN_HEADERS} ${PLUGIN_SOURCES})

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
endif()

if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
	set(D3D12_HEADERS	D3D12/D3D12Device.h
						D3D12/D3D12SwapChain.h
						D3D12/D3D12RenderTarget.h
						D3D12/D3D12DepthStencilBuffer.h
						D3D12/D3D12Renderer.h
						D3D12/D3D12RenderFence.h
						D3D12/D3D12DescriptorHeapManager.h
						D3D12/D3D12TypeMapper.h
						D3D12/D3D12Shader.h
						D3D12/D3D12BufferResource.h
						D3D12/D3D12VertexBuffer.h
						D3D12/D3D12IndexBuffer.h
						D3D12/D3D12ConstantBufferManager.h
						D3D12/D3D12CommandEncoder.h
						D3D12/D3D12StatefulResource.h
						D3D12/D3D12StatefulResourceManager.h
						D3D12/D3D12ForwardRenderPass.h
						D3D12/D3D12Texture.h)
	set(D3D12_SOURCES	D3D12/D3D12Device.cpp
						D3D12/D3D12SwapChain.cpp
						D3D12/D3D12RenderTarget.cpp
						D3D12/D3D12DepthStencilBuffer.cpp
						D3D12/D3D12Renderer.cpp
						D3D12/D3D12RenderFence.cpp
						D3D12/D3D12DescriptorHeapManager.cpp
						D3D12/D3D12Shader.cpp
						D3D12/D3D12BufferResource.cpp
						D3D12/D3D12VertexBuffer.cpp
						D3D12/D3D12IndexBuffer.cpp
						D3D12/D3D12ConstantBufferManager.cpp
						D3D12/D3D12CommandEncoder.cpp
						D3D12/D3D12StatefulResource.cpp
						D3D12/D3D12StatefulResourceManager.cpp
						D3D12/D3D12ForwardRenderPass.cpp
						D3D12/D3D12Texture.cpp)
	list(APPEND HEADERS ${D3D12_HEADERS})
	list(APPEND SOURCES ${D3D12_SOURCES})
	source_group("D3D12" FILES ${D3D12_HEADERS} ${D3D12_SOURCES})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

define_dll_export_macro(RENDERER)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}/Types
					${CMAKE_CURRENT_SOURCE_DIR}/Shader
					${CMAKE_CURRENT_SOURCE_DIR}/Texture
					${CMAKE_SOURCE_DIR}/PluginSystem
					${CMAKE_SOURCE_DIR}/Foundation
					${CMAKE_SOURCE_DIR}/Foundation/Memory
					${CMAKE_SOURCE_DIR}/Foundation/Math
					${CMAKE_SOURCE_DIR}/Platform 
					${CMAKE_SOURCE_DIR}/Window
					${CMAKE_SOURCE_DIR}/Loader
					${FREEIMAGE_INCLUDE_DIR}
					${Boost_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})

if(LIGHTNING_USE_D3D12)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/D3D12
						${LIGHTNING_DEPENDENCIES_DIR}/D3D12)
endif()

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
target_link_libraries(${PROJECT_NAME} ${FREEIMAGE_LIBRARIES})
add_dependencies(${PROJECT_NAME} Foundation Window Loader)
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
if(LIGHTNING_USE_D3D12)
	target_link_libraries(${PROJECT_NAME} dxgi.lib d3d12.lib d3dcompiler.lib)
	target_link_libraries(${PROJECT_NAME} dxguid.lib)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FREEIMAGE_DLLS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FREEIMAGE_DLLS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
