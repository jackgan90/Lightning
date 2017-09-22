project(renderer)
message(STATUS "Configuring project renderer")

set(HEADERS rendererexportdef.h
			idevice.h
			irendercontext.h
			iswapchain.h)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32)
endif()

if(LIGHTNINGGE_USE_D3D12)
	set(D3D12_HEADERS	"d3d12/d3d12device.h"
						"d3d12/d3d12rendercontext.h"
						"d3d12/d3d12swapchain.h")
	set(D3D12_SOURCES	"d3d12/d3d12device.cpp"
						"d3d12/d3d12rendercontext.cpp"
						"d3d12/d3d12swapchain.cpp")
	list(APPEND HEADERS ${D3D12_HEADERS})
	list(APPEND SOURCES ${D3D12_SOURCES})
	source_group("d3d12" FILES ${D3D12_HEADERS} ${D3D12_SOURCES})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
endif()

define_dll_export_macro(RENDERER)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/platform 
					${CMAKE_SOURCE_DIR}/platform/window)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
add_dependencies(${PROJECT_NAME} foundation utilities platform)
target_link_libraries(${PROJECT_NAME} foundation utilities platform)
