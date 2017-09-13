cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH 
  "${CMAKE_SOURCE_DIR}/CMake"
)
include(helpers)

project(LightningGE)
add_subdirectory(foundation)
add_subdirectory(platform)
add_subdirectory(utilities)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (WIN32)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
endif()

if (WIN32)
	add_definitions("-DLIGHTNINGGE_WIN32")
endif()

set(SOURCE_FILES entry.cpp)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${LIGHTNINGGE_LINKER_FLAGS})

include_directories("foundation" "utilities" "platform")
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} foundation utilities platform)

target_link_libraries(${PROJECT_NAME} utilities platform)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
