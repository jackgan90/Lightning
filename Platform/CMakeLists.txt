project(Platform)
message(STATUS "Configuring project platform")

set(APPLICATION_SUBSYSTEM_DIR "Application")

set(HEADERS	PlatformExportDef.h)

set(APPLICATION_HEADERS		${APPLICATION_SUBSYSTEM_DIR}/AppSystem.h
							${APPLICATION_SUBSYSTEM_DIR}/AppComponent.h)
set(APPLICATION_SOURCES		${APPLICATION_SUBSYSTEM_DIR}/AppSystem.cpp)


if(WIN32)
	list(APPEND APPLICATION_HEADERS ${APPLICATION_SUBSYSTEM_DIR}/Win32AppSystem.h)
	list(APPEND APPLICATION_SOURCES ${APPLICATION_SUBSYSTEM_DIR}/Win32AppSystem.cpp)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
	include_directories(${CMAKE_SOURCE_DIR}/render/d3d12)
endif()

source_group("Application" FILES 
			${APPLICATION_HEADERS}
			${APPLICATION_SOURCES})

list(APPEND HEADERS ${APPLICATION_HEADERS})
list(APPEND SOURCES ${APPLICATION_SOURCES})

define_dll_export_macro(PLATFORM)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/foundation
					${CMAKE_SOURCE_DIR}/foundation/memory
					${CMAKE_SOURCE_DIR}/foundation/math
					${CMAKE_CURRENT_SOURCE_DIR} 
					${CMAKE_SOURCE_DIR}/render
					${CMAKE_SOURCE_DIR}/render/types
					${CMAKE_SOURCE_DIR}/render/shader
					${CMAKE_SOURCE_DIR}/window
					${CMAKE_SOURCE_DIR}/utilities
					${CMAKE_SOURCE_DIR}/scene
					${CMAKE_SOURCE_DIR}/loader
					${CMAKE_SOURCE_DIR}/entities
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})


add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} Foundation Window Render Scene Loader)

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
target_link_libraries(${PROJECT_NAME} Foundation Window Render Scene Loader)

