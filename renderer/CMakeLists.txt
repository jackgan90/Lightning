project(renderer)
message(STATUS "Configuring project renderer")

set(HEADERS rendererexportdef.h
			idevice.h
			irendercontext.h
			iswapchain.h
			irendertarget.h
			irendertargetmanager.h
			rendererfactory.h
			irenderresourceuser.h
			irenderer.h
			renderer.h)
set(SOURCES rendererfactory.cpp
			renderer.cpp)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()

if(LIGHTNINGGE_USE_D3D12)
	set(D3D12_HEADERS	d3d12/d3d12device.h
						d3d12/d3d12rendercontext.h
						d3d12/d3d12swapchain.h
						d3d12/d3d12rendertarget.h
						d3d12/d3d12rendertargetmanager.h
						d3d12/d3d12renderer.h)
	set(D3D12_SOURCES	d3d12/d3d12device.cpp
						d3d12/d3d12rendercontext.cpp
						d3d12/d3d12swapchain.cpp
						d3d12/d3d12rendertarget.cpp
						d3d12/d3d12rendertargetmanager.cpp
						d3d12/d3d12renderer.cpp)
	list(APPEND HEADERS ${D3D12_HEADERS})
	list(APPEND SOURCES ${D3D12_SOURCES})
	source_group("d3d12" FILES ${D3D12_HEADERS} ${D3D12_SOURCES})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

define_dll_export_macro(RENDERER)
include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost
					${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/platform 
					${CMAKE_SOURCE_DIR}/window)
if(LIGHTNINGGE_USE_D3D12)
	include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/d3d12)
endif()

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
add_dependencies(${PROJECT_NAME} foundation window)
target_link_libraries(${PROJECT_NAME} foundation window)
if(LIGHTNINGGE_USE_D3D12)
	target_link_libraries(${PROJECT_NAME} dxgi.lib d3d12.lib)
	target_link_libraries(${PROJECT_NAME} debug dxguid.lib)
endif()
