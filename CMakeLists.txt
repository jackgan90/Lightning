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

add_subdirectory(foundation)
add_subdirectory(utilities)
add_subdirectory(platform)
add_subdirectory(window)
add_subdirectory(render)
add_subdirectory(unittest)
add_subdirectory(loader)
add_subdirectory(scene)
add_subdirectory(jobsystem)



if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
endif()

set(LIBRARY_HEADERS	libloader/libexportdef.h
					libloader/library.h
					libloader/librarymgr.h)

set(LIBRARY_SOURCES	libloader/library.cpp
					libloader/librarymgr.cpp)

set(SOURCE_FILES	entry.cpp)

list(APPEND HEADER_FILES ${LIBRARY_HEADERS})
list(APPEND SOURCE_FILES ${LIBRARY_SOURCES})

source_group("libloader" FILES ${LIBRARY_HEADERS} ${LIBRARY_SOURCES})

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${LIGHTNING_LINKER_FLAGS})
define_dll_export_macro(LIB)


include_directories(${CMAKE_SOURCE_DIR}/foundation 
					${CMAKE_SOURCE_DIR}/foundation/memory
					${CMAKE_SOURCE_DIR}/foundation/math
					${CMAKE_SOURCE_DIR}/platform 
					${CMAKE_SOURCE_DIR}/platform/application 
					${CMAKE_SOURCE_DIR}/window
					${CMAKE_SOURCE_DIR}/render
					${CMAKE_SOURCE_DIR}/render/types
					${CMAKE_SOURCE_DIR}/render/shader
					${CMAKE_SOURCE_DIR}/render/texture
					${CMAKE_SOURCE_DIR}/utilities
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR}
					${SPDLOG_INCLUDE_DIR})

add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} foundation platform render)
target_link_libraries(${PROJECT_NAME} platform render)
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
