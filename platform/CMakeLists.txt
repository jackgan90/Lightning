project(platform)
message(STATUS "Configuring project platform")

set(APPLICATION_SUBSYSTEM_DIR "application")



set(APPLICATION_HEADERS		${APPLICATION_SUBSYSTEM_DIR}/appsystem.h
							${APPLICATION_SUBSYSTEM_DIR}/appcomponent.h)
set(APPLICATION_SOURCES		${APPLICATION_SUBSYSTEM_DIR}/appsystem.cpp)

if(WIN32)
	list(APPEND APPLICATION_HEADERS ${APPLICATION_SUBSYSTEM_DIR}/win32appsystem.h)
	list(APPEND APPLICATION_SOURCES ${APPLICATION_SUBSYSTEM_DIR}/win32appsystem.cpp)
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

source_group("application" FILES 
			${APPLICATION_HEADERS}
			${APPLICATION_SOURCES})

set(HEADERS platformexportdef.h)
list(APPEND HEADERS ${APPLICATION_HEADERS})
set(SOURCES ${APPLICATION_SOURCES})

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
					${LIGHTNING_DEPENDENCIES_DIR}/spdlog/include
					${LIGHTNING_DEPENDENCIES_DIR}/boost
					${LIGHTNING_DEPENDENCIES_DIR}/tbb/include)

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} foundation window render scene loader)

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
target_link_libraries(${PROJECT_NAME} foundation window render scene loader)

