project(foundation)
message(STATUS "Configuring project foundation")

set(MEMORY_HEADERS	memory/memoryexportdef.h
					memory/memorymacros.h
					memory/imemoryallocator.h
					memory/basememoryallocator.h
					memory/allocatormanager.h
					memory/memorysystem.h)

set(MEMORY_SOURCES	memory/basememoryallocator.cpp
					memory/allocatormanager.cpp)

set(HEADERS	common.h
			timesystem.h
			filesystem.h
			logger.h
			filesystemfactory.h
			configmanager.h
			foundationexportdef.h
			singleton.h)
set(SOURCES common.cpp
			filesystem.cpp
			logger.cpp
			configmanager.cpp
			filesystemfactory.cpp
			timesystem.cpp)

list(APPEND HEADERS ${MEMORY_HEADERS})
list(APPEND SOURCES ${MEMORY_SOURCES})

source_group("memory" FILES	${MEMORY_HEADERS} ${MEMORY_SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	add_definitions(-DLIGHTNINGGE_WIN32 
					-DMEMORY_PLATFORM_WIN32)
endif()
add_definitions(-DLIGHTNINGGE_INTERNAL_LOGGER
				-DMEMORY_EXPORT
				-DBOOST_FILESYSTEM_NO_DEPRECATED)

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
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG")
	set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG)
endif()
