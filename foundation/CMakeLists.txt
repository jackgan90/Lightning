project(foundation)
message(STATUS "Configuring project foundation")

set(HEADERS	common.h
			timesystem.h
			filesystem.h
			logger.h
			filesystemfactory.h
			configmanager.h
			foundationexportdef.h)
set(SOURCES common.cpp
			filesystem.cpp
			logger.cpp
			configmanager.cpp
			filesystemfactory.cpp
			timesystem.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32)
	add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif()
add_definitions(-DLIGHTNINGGE_INTERNAL_LOGGER)

define_dll_export_macro(FOUNDATION)

include_directories(${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
endif()

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${LIGHTNINGGE_DEPENDENCIES_DIR}/boost)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LIGHTNINGGE_DEPENDENCIES_DIR}/boost/lib)
find_package(Boost 1.65.1 REQUIRED)
if(Boost_FOUND)
	add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif()

if(MSVC)
	get_msvc_postfix(POSTFIX)
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARY_DIRS}/libboost_filesystem-vc${POSTFIX}-mt-1_65_1.lib)
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARY_DIRS}/libboost_system-vc${POSTFIX}-mt-1_65_1.lib)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
