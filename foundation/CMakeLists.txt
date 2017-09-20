project(foundation)
message(STATUS "Configuring project foundation")

set(HEADERS	common.h
			timesystem.h
			filesystem.h
			foundationexportdef.h)
set(SOURCES common.cpp
			filesystem.cpp
			timesystem.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (WIN32)
	set(PROJECT_COMPILE_FLAGS "/wd4251")
endif()
define_dll_export_macro(FOUNDATION)

include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS})
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIGHTNINGGE_DEPENDENCIES_DIR}/boost/lib)
find_package(Boost 1.65.1 REQUIRED)
if(Boost_FOUND)
	add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif()

if(MSVC)
	get_msvc_postfix(POSTFIX)
	target_link_libraries(${PROJECT_NAME} debug ${Boost_LIBRARY_DIRS}/libboost_system-vc${POSTFIX}-mt-gd-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} debug ${Boost_LIBRARY_DIRS}/libboost_filesystem-vc${POSTFIX}-mt-gd-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} optimized ${Boost_LIBRARY_DIRS}/libboost_filesystem-vc${POSTFIX}-mt-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} optimized ${Boost_LIBRARY_DIRS}/libboost_system-vc${POSTFIX}-mt-1_65_1.lib)
endif()
