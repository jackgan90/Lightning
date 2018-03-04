project(render)
message(STATUS "Configuring project renderer")


set(HEADERS rendererexportdef.h
			idevice.h
			device.h
			iswapchain.h
			irendertarget.h
			irendertargetmanager.h
			idepthstencilbuffer.h
			irenderer.h
			renderer.h
			renderstates.h
			semantics.h
			vertexbuffer.h
			indexbuffer.h
			rendererhelper.h
			renderexception.h
			rendererfactory.h
			gpubuffer.h
			bindablebuffer.h
			renderconstants.h
			geometry.h
			material.h
			renderpass.h
			forwardrenderpass.h
			deferedrenderpass.h
			transform.h)
set(SOURCES renderer.cpp
			device.cpp
			vertexbuffer.cpp
			indexbuffer.cpp
			rendererhelper.cpp
			rendererfactory.cpp
			renderconstants.cpp
			rendertargetmanager.cpp
			gpubuffer.cpp
			bindablebuffer.cpp
			geometry.cpp
			material.cpp
			forwardrenderpass.cpp
			deferedrenderpass.cpp
			transform.cpp)

set(TYPES_HEADERS	types/color.h
					types/rect.h
					types/vector.h
					types/matrix.h)

set(SHADER_HEADERS	shader/ishader.h
					shader/shaderdefine.h
					shader/ishadermanager.h
					shader/shadermanager.h)
set(SHADER_SOURCES	shader/shaderdefine.cpp
					shader/shadermanager.cpp
					shader/shadermanager.cpp
					shader/shader.cpp)

list(APPEND HEADERS ${TYPES_HEADERS} ${SHADER_HEADERS})
list(APPEND SOURCES ${SHADER_SOURCES})

source_group("types" FILES ${TYPES_HEADERS})
source_group("shader" FILES 
							${SHADER_HEADERS}
							${SHADER_SOURCES})

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()
if(LIGHTNINGGE_USE_D3D12)
	add_definitions(-DLIGHTNINGGE_USE_D3D12)
endif()

if(LIGHTNINGGE_USE_D3D12)
	add_definitions(-DLIGHTNINGGE_USE_D3D12)
	set(D3D12_HEADERS	d3d12/d3d12device.h
						d3d12/d3d12swapchain.h
						d3d12/d3d12rendertarget.h
						d3d12/d3d12depthstencilbuffer.h
						d3d12/d3d12rendertargetmanager.h
						d3d12/d3d12renderer.h
						d3d12/d3d12descriptorheapmanager.h
						d3d12/d3d12typemapper.h
						d3d12/d3d12shader.h
						d3d12/d3d12shadermanager.h)
	set(D3D12_SOURCES	d3d12/d3d12device.cpp
						d3d12/d3d12swapchain.cpp
						d3d12/d3d12rendertarget.cpp
						d3d12/d3d12depthstencilbuffer.cpp
						d3d12/d3d12rendertargetmanager.cpp
						d3d12/d3d12renderer.cpp
						d3d12/d3d12descriptorheapmanager.cpp
						d3d12/d3d12shader.cpp
						d3d12/d3d12shadermanager.cpp)
	list(APPEND HEADERS ${D3D12_HEADERS})
	list(APPEND SOURCES ${D3D12_SOURCES})
	source_group("d3d12" FILES ${D3D12_HEADERS} ${D3D12_SOURCES})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

define_dll_export_macro(RENDERER)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}/types
					${CMAKE_CURRENT_SOURCE_DIR}/shader
					${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/foundation/memory
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/platform 
					${CMAKE_SOURCE_DIR}/window
					${LIGHTNINGGE_DEPENDENCIES_DIR}/boost
					${LIGHTNINGGE_DEPENDENCIES_DIR}/eigen)
if(LIGHTNINGGE_USE_D3D12)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/d3d12
						${LIGHTNINGGE_DEPENDENCIES_DIR}/d3d12)
endif()

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd /arch:SSE2")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD /arch:SSE2")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /arch:SSE2")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /arch:SSE2")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
add_dependencies(${PROJECT_NAME} foundation window utilities)
target_link_libraries(${PROJECT_NAME} foundation window utilities)
if(LIGHTNINGGE_USE_D3D12)
	target_link_libraries(${PROJECT_NAME} dxgi.lib d3d12.lib d3dcompiler.lib)
	target_link_libraries(${PROJECT_NAME} dxguid.lib)
endif()
