cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH 
  "${CMAKE_SOURCE_DIR}/CMake"
)
include(helpers)

# options start
option(LIGHTNINGGE_USE_D3D12 "Enable DirectX 12 renderer." ON)
set(LIGHTNINGGE_DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies" CACHE PATH "Dependencies directory")
get_filename_component(LIGHTNINGGE_DEPENDENCIES_DIR ${LIGHTNINGGE_DEPENDENCIES_DIR} ABSOLUTE)
message(STATUS "Dependencies directory is ${LIGHTNINGGE_DEPENDENCIES_DIR}")
# options end

project(LightningGE)
message(STATUS "Configuring project LightningGE")
add_subdirectory(foundation)
add_subdirectory(utilities)
add_subdirectory(platform)
add_subdirectory(window)
add_subdirectory(renderer)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32
					-DMEMORY_PLATFORM_WIN32)
endif()
if(LIGHTNINGGE_USE_D3D12)
	add_definitions(-DLIGHTNINGGE_USE_D3D12)
endif()

set(SOURCE_FILES entry.cpp)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${LIGHTNINGGE_LINKER_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/foundation 
					${CMAKE_SOURCE_DIR}/foundation/memory
					${CMAKE_SOURCE_DIR}/platform 
					${CMAKE_SOURCE_DIR}/platform/application 
					${CMAKE_SOURCE_DIR}/window
					${LIGHTNINGGE_DEPENDENCIES_DIR}/boost
					${CMAKE_SOURCE_DIR}/renderer
					${CMAKE_SOURCE_DIR}/renderer/types
					${CMAKE_SOURCE_DIR}/renderer/shader
					${CMAKE_SOURCE_DIR}/utilities
					${LIGHTNINGGE_DEPENDENCIES_DIR}/eigen)
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} foundation platform renderer)
target_link_libraries(${PROJECT_NAME} platform renderer)
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4251 /wd4275 /EHsc /MDd")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4251 /wd4275 /EHsc /MD")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4251 /wd4275 /EHsc")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
