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
	add_definitions("-DLIGHTNINGGE_WIN32")
endif()

set(SOURCE_FILES entry.cpp)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${LIGHTNINGGE_LINKER_FLAGS})

include_directories(foundation platform platform/application window)
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} foundation platform renderer)
target_link_libraries(${PROJECT_NAME} platform renderer)
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
