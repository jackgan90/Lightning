cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH 
  "${CMAKE_SOURCE_DIR}/CMake"
)
include(helpers)

# options start
option(LIGHTNING_USE_D3D12 "Enable DirectX 12 renderer." ON)
set(LIGHTNING_DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/../Lightning_dependency" CACHE PATH "Dependencies directory")
get_filename_component(LIGHTNING_DEPENDENCIES_DIR ${LIGHTNING_DEPENDENCIES_DIR} ABSOLUTE)
message(STATUS "Dependencies directory is ${LIGHTNING_DEPENDENCIES_DIR}")
# options end

project(Lightning)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
message(STATUS "Configuring project Lightning")

#find boost
set(BOOST_ROOT ${LIGHTNING_DEPENDENCIES_DIR}/boost)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.68.0 REQUIRED COMPONENTS system filesystem)
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

#find rttr
set(RTTR_ROOT ${LIGHTNING_DEPENDENCIES_DIR}/rttr)
find_package(rttr REQUIRED)

#find tbb
set(TBB_INSTALL_DIR ${LIGHTNING_DEPENDENCIES_DIR}/tbb)
set(TBB_ROOT ${LIGHTNING_DEPENDENCIES_DIR}/tbb)
find_package(tbb REQUIRED)

#find freeimage
set(FREEIMAGE_ROOT ${LIGHTNING_DEPENDENCIES_DIR}/FreeImage)
find_package(FreeImage REQUIRED)

#spdlog
set(SPDLOG_INCLUDE_DIR ${LIGHTNING_DEPENDENCIES_DIR}/spdlog/include)

add_subdirectory(Foundation)
add_subdirectory(Platform)
add_subdirectory(Window)
add_subdirectory(Render)
add_subdirectory(Unittest)
add_subdirectory(Loader)
add_subdirectory(World)
add_subdirectory(ResourceGenerator)
#add_subdirectory(JobSystem)



if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
endif()

set(LIBRARY_HEADERS	PluginSystem/IPlugin.h
					PluginSystem/Plugin.h
					PluginSystem/IPluginManager.h
					PluginSystem/RefObject.h
					PluginSystem/IRefObject.h
					PluginSystem/Portable.h
					PluginMgrImpl/PluginManager.h)

set(LIBRARY_SOURCES	PluginMgrImpl/PluginManager.cpp)

set(HEADER_FILES	Engine.h)
set(SOURCE_FILES	Engine.cpp
					Entry.cpp)

list(APPEND HEADER_FILES ${LIBRARY_HEADERS})
list(APPEND SOURCE_FILES ${LIBRARY_SOURCES})

source_group("PluginSystem" FILES ${LIBRARY_HEADERS} ${LIBRARY_SOURCES})

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${LIGHTNING_LINKER_FLAGS})
define_dll_export_macro(PLUGIN)


include_directories(${CMAKE_SOURCE_DIR}/Foundation 
					${CMAKE_SOURCE_DIR}/Foundation/Memory
					${CMAKE_SOURCE_DIR}/Foundation/Math
					${CMAKE_SOURCE_DIR}/Platform 
					${CMAKE_SOURCE_DIR}/Platform/Application 
					${CMAKE_SOURCE_DIR}/Window
					${CMAKE_SOURCE_DIR}/Render
					${CMAKE_SOURCE_DIR}/Render/Types
					${CMAKE_SOURCE_DIR}/Render/Shader
					${CMAKE_SOURCE_DIR}/Render/Texture
					${CMAKE_SOURCE_DIR}/PluginSystem
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})

add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} Foundation Platform Render)
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})

if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /MP")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc /MP")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
