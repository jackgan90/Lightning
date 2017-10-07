project(renderer)
message(STATUS "Configuring project renderer")


set(HEADERS rendererexportdef.h
			idevice.h
			irendercontext.h
			iswapchain.h
			irendertarget.h
			irendertargetmanager.h
			rendererfactory.h
			irenderresourcekeeper.h
			irenderer.h
			renderer.h
			iblendstate.h
			idepthstencilstate.h
			ipipelinestateobject.h
			semantics.h
			ivertexbuffer.h
			vertexbuffer.h
			rendererhelper.h
			irasterizerstate.h)
set(SOURCES rendererfactory.cpp
			renderer.cpp
			vertexbuffer.cpp
			rendererhelper.cpp
			blendstate.cpp
			depthstencilstate.cpp
			rasterizerstate.cpp
			pipelinestateobject.cpp)

set(TYPES_HEADERS	types/color.h
					types/rect.h)

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
	set(D3D12_HEADERS	d3d12/d3d12device.h
						d3d12/d3d12rendercontext.h
						d3d12/d3d12swapchain.h
						d3d12/d3d12rendertarget.h
						d3d12/d3d12rendertargetmanager.h
						d3d12/d3d12renderer.h
						d3d12/d3d12descriptorheapmanager.h
						d3d12/d3d12blendstate.h
						d3d12/d3d12depthstencilstate.h
						d3d12/d3d12pipelinestateobject.h
						d3d12/d3d12typemapper.h
						d3d12/d3d12shader.h
						d3d12/d3d12shadermanager.h
						d3d12/d3d12rasterizerstate.h)
	set(D3D12_SOURCES	d3d12/d3d12device.cpp
						d3d12/d3d12rendercontext.cpp
						d3d12/d3d12swapchain.cpp
						d3d12/d3d12rendertarget.cpp
						d3d12/d3d12rendertargetmanager.cpp
						d3d12/d3d12renderer.cpp
						d3d12/d3d12descriptorheapmanager.cpp
						d3d12/d3d12blendstate.cpp
						d3d12/d3d12depthstencilstate.cpp
						d3d12/d3d12pipelinestateobject.cpp
						d3d12/d3d12shader.cpp
						d3d12/d3d12shadermanager.cpp
						d3d12/d3d12rasterizerstate.cpp)
	list(APPEND HEADERS ${D3D12_HEADERS})
	list(APPEND SOURCES ${D3D12_SOURCES})
	source_group("d3d12" FILES ${D3D12_HEADERS} ${D3D12_SOURCES})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

define_dll_export_macro(RENDERER)
include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost
					${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_CURRENT_SOURCE_DIR}/types
					${CMAKE_CURRENT_SOURCE_DIR}/shader
					${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/platform 
					${CMAKE_SOURCE_DIR}/window
					${LIGHTNINGGE_DEPENDENCIES_DIR}/eigen)
if(LIGHTNINGGE_USE_D3D12)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/d3d12
						${LIGHTNINGGE_DEPENDENCIES_DIR}/d3d12)
endif()

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
add_dependencies(${PROJECT_NAME} foundation window utilities)
target_link_libraries(${PROJECT_NAME} foundation window utilities)
if(LIGHTNINGGE_USE_D3D12)
	target_link_libraries(${PROJECT_NAME} dxgi.lib d3d12.lib d3dcompiler.lib)
	target_link_libraries(${PROJECT_NAME} debug dxguid.lib)
endif()
