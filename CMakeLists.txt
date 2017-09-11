cmake_minimum_required(VERSION 2.8)

project(LightningGE)
add_subdirectory(foundation)
add_subdirectory(platform)

if (WIN32)
	add_definitions("-DLIGHTNINGGE_WIN32")
endif()

set(SOURCE_FILES entry.cpp)

set(LIGHTNINGGE_LINKER_FLAGS "/MD")

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${LIGHTNINGGE_LINKER_FLAGS})

include_directories("foundation")
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} foundation platform)

target_link_libraries(${PROJECT_NAME} "platform/Debug/platform")
